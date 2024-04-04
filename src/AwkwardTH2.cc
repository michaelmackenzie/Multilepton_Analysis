#include "interface/AwkwardTH2.hh"

using namespace CLFV;

//----------------------------------------------------------------------------------------
AwkwardTH2::AwkwardTH2(const char* name, const char* title, std::map<double, std::vector<double>> binning, const char type) : h_(nullptr), hAxis_(nullptr), type_(type) {
  if(binning.size() <= 1) {
    printf("%s: Size %i binning given!\n", __func__, (int) binning.size());
    return;
  }
  //retrieve the list of edges and the N(bins)
  bin_edges_ = {};
  std::map<double, std::vector<double>>::iterator it;
  for(it = binning.begin(); it != binning.end(); it++) {
    const double bin_low_edge = it->first;
    bin_edges_.push_back(bin_low_edge);
  }
  //sort the list of edges
  std::sort(bin_edges_.begin(), bin_edges_.end());

  //the last bin doesn't define any y-bin edges
  binning_[bin_edges_.back()] = {};

  //get the N(bins) after ensuring the last bin has no edges
  int nbins = 0;
  for(unsigned x_bin = 0; x_bin < bin_edges_.size() - 1 /*no need to check last bin*/; ++x_bin) {
    const double& bin_low_edge = bin_edges_[x_bin];
    std::vector<double>& y_edges = binning[bin_low_edge];
    if(y_edges.size() < 2) {
      printf("%s: Fewer than two bin edges given for x bin %f (size %lu)\n", __func__, bin_low_edge, y_edges.size());
      bin_edges_ = {};
      return;
    }
    nbins += y_edges.size() + 1; //N(edges) - 1 bins + under/overflow bins
  }
  binning_ = binning;

  //create the unrolled 1D histogram
  if(type == 'D') {
    h_ = new TH1D(name, title, nbins, 0, nbins);
  } else if(type == 'F') {
    h_ = new TH1F(name, title, nbins, 0, nbins);
  } else {
    printf("%s: Unknown TH1 type: %c\n", __func__, type);
  }
}


//----------------------------------------------------------------------------------------
int AwkwardTH2::FindBin(double x, double y) {
  //check for under/overflow
  if(x <  bin_edges_[0]    ) return 0;
  if(x >= bin_edges_.back()) return h_->GetNbinsX() + 1;

  //linear search for the bin, to accumulate N(bins)
  unsigned x_bin = 0;
  unsigned bin_offset = 1;  //start within x-axis region, outside of underflow
  for(unsigned i_bin = 1; i_bin < bin_edges_.size(); ++i_bin) {
    if(x >= bin_edges_[i_bin]) {
      x_bin = i_bin;
      //add N(bins) from previous x-bin
      bin_offset += binning_[bin_edges_[i_bin-1]].size() + 1; //N(edges) - 1 + under/overflow per x-axis bin
    } else { //contained in the previous bin
      break;
    }
  }

  //search within the x-axis bin for the y-axis bin
  unsigned y_bin = 0;
  const std::vector<double>& ybins = binning_[bin_edges_[x_bin]];
  for(unsigned i_bin = 0; i_bin < ybins.size(); ++i_bin) {
    if(y >= ybins[i_bin]) {
      y_bin += 1; //move up a bin
    } else { //contained in the previous bin
      break;
    }
  }

  const int bin = (y_bin + bin_offset);
  if(bin <= 0 || bin >= h_->GetNbinsX() + 1) {
    printf("AwkwardTH2::%s: Bin finding error! y_bin = %i and bin_offset = %i\n", __func__, y_bin, bin_offset);
    return 0;
  }
  return y_bin + bin_offset;
}

//----------------------------------------------------------------------------------------
int AwkwardTH2::Fill(double x, double y, double wt) {
  const int ibin = FindBin(x,y);
  return h_->Fill(ibin-0.5, wt); //fill the corresponding bin, offset by -0.5 to ensure filling at bin centers
}

//----------------------------------------------------------------------------------------
double AwkwardTH2::Integral() {
  //Get the integral without under/overflow for each x-bin
  double integral = 0.;
  unsigned bin_offset = 1; //start after the x-axis underflow
  for(unsigned x_bin = 0; x_bin < bin_edges_.size() -1; ++x_bin) {
    const std::vector<double>& y_edges = binning_[bin_edges_[x_bin]];
    integral += h_->Integral(x_bin + bin_offset, x_bin + y_edges.size() - 2 + bin_offset); //2 edges correspond to 1 bin
    bin_offset += y_edges.size() + 1; //N(bins) = N(edges) - 1 + under/overflow
  }
  return integral;
}

//----------------------------------------------------------------------------------------
double AwkwardTH2::Integral(int low_bin, int high_bin) {
  return h_->Integral(low_bin, high_bin);
}

//----------------------------------------------------------------------------------------
double AwkwardTH2::Integral(double xmin, double xmax, double ymin, double ymax) {
  double integral = 0.;
  //loop through the bins
  for(unsigned x_bin = 0; x_bin < bin_edges_.size() - 1; ++x_bin) {
    const double x = bin_edges_[x_bin];
    if(x < xmin || x >= xmax) continue;
    const std::vector<double>& y_edges = binning_[x];
    for(unsigned y_bin = 0; y_bin < y_edges.size() - 1; ++y_bin) {
      const double y = y_edges[y_bin];
      if(y < ymin || y >= ymax) continue;
      integral += h_->GetBinContent(FindBin(x,y));
    }
  }
  return integral;
}

//----------------------------------------------------------------------------------------
void AwkwardTH2::Draw(const char* option) {
  //Create a 2D histogram with the correct x-axis binning but 1 y-axis bin
  double ymin(1.e20), ymax(-1.e20);
  for(double x : bin_edges_) {
    if(binning_[x].size() == 0) continue;
    ymin = std::min(ymin, binning_[x][0]);
    ymax = std::max(ymax, binning_[x].back());
  }
  if(!hAxis_) {
    hAxis_ = new TH2F(Form("%s_axis", h_->GetName()), h_->GetTitle(), bin_edges_.size() - 1, bin_edges_.data(), 1, ymin, ymax);
  }

  TString opt_s(option);
  opt_s.ToLower();

  //Draw the axis histogram first
  hAxis_->SetLineColor(0);
  hAxis_->SetFillColor(0);
  hAxis_->SetMarkerSize(0);
  // hAxis_->Draw((opt_s.Contains("same") ? "same" : ""));
  hAxis_->Draw(opt_s.Data());

  //turn off Z-axis for next histograms
  opt_s.ReplaceAll("colz", "col");

  //Create a histogram for each x-axis bin
  double max_val(-1.e20), min_val(1.e20);
  for(unsigned x_bin = 0; x_bin < bin_edges_.size() - 1; ++x_bin) {
    const double x = bin_edges_[x_bin];
    const std::vector<double>& y_bins = binning_[x];
    const int lo_bin = FindBin(x, y_bins[0]);
    const int hi_bin = FindBin(x, (y_bins[y_bins.size()-1] + y_bins.back())/2.);
    TH2* h(nullptr);
    if     (type_ == 'D') h = new TH2D(Form("%s_bin_%i", h_->GetName(), x_bin), "", 1, x, bin_edges_[x_bin+1], y_bins.size() - 1, y_bins.data());
    else if(type_ == 'F') h = new TH2F(Form("%s_bin_%i", h_->GetName(), x_bin), "", 1, x, bin_edges_[x_bin+1], y_bins.size() - 1, y_bins.data());
    else {
      printf("AwkwardTH2::%s: Unknown histogram type %c\n", __func__, type_);
      return;
    }
    for(int i_bin = lo_bin; i_bin <= hi_bin; ++i_bin) {
      h->SetBinContent(1, i_bin - lo_bin + 1, h_->GetBinContent(i_bin));
      h->SetBinError  (1, i_bin - lo_bin + 1, h_->GetBinError  (i_bin));
      max_val = std::max(max_val, h_->GetBinContent(i_bin));
      min_val = std::min(min_val, h_->GetBinContent(i_bin));
    }
    h->Draw(Form("same %s", opt_s.Data()));
  }

  if(!opt_s.Contains("same")) {
    const double val_diff = max_val - min_val;
    const double z_min = (min_val >= 0.) ? std::max(0., min_val - 0.05*val_diff) : min_val - 0.05*val_diff;
    const double z_max = max_val + 0.05*val_diff;
    hAxis_->GetZaxis()->SetRangeUser(z_min, z_max);
  }
}
