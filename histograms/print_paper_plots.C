//Print the plots used in the paper
#include "dataplotter_clfv.C"

int print_paper_plots(TString selection = "mutau", TString hist_dir = "nanoaods_postfit",
                      vector<int> years = {2016,2017,2018},
                      TString out_dir = "paper",
                      bool add_sys = false) {
  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (selection == "emu") ? 0 : 2;
  drawStats_     = false; //data and MC total integrals
  doStatsLegend_ = false; //process yields in the legend

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  dataplotter_->figure_format_ = "png"; //save PDF files
  dataplotter_->print_root_canvas_ = true; //add root files with the canvases for easier paper updates
  dataplotter_->plot_y_title_ = 1; //add y-axis label
  dataplotter_->add_y_unit_ = 1;
  dataplotter_->cms_is_prelim_ = true; //not preliminary
  dataplotter_->lum_txt_x_ = 0.00;
  dataplotter_->lum_txt_y_ = 0.945;
  dataplotter_->cms_txt_x_ = 0.00;
  dataplotter_->cms_txt_y_ = 0.945;
  dataplotter_->x_title_offset_ = 0.85;
  dataplotter_->y_title_offset_ = 0.45;
  dataplotter_->x_label_size_ = 0.1;
  dataplotter_->y_label_size_ = 0.1;
  dataplotter_->x_label_offset_ = 0.015;
  dataplotter_->y_label_offset_ = 0.02;
  dataplotter_->lower_pad_y1_ = 0.;
  dataplotter_->lower_pad_botmargin_ = 0.33;
  TGaxis::SetMaxDigits(6);

  dataplotter_->misid_color_ = kCMSColor_1;
  dataplotter_->qcd_color_   = kCMSColor_2;

  //print each figure
  if(selection == "emu") { //Z->e+mu
    print_mass(8, add_sys);
    print_mva(8, add_sys);
  } else { //Z->X+tau
    print_collinear_mass(8, add_sys);
    print_lep_beta(8, add_sys);
    dataplotter_->plot_y_title_ = 0; //add y-axis label
    dataplotter_->add_y_unit_ = 0;
    print_mva(8, add_sys);
  }
  return status;
}
