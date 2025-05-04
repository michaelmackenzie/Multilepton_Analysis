//Print the plots used in the paper
#include "dataplotter_clfv.C"

int print_paper_plots(TString selection = "mutau", TString hist_dir = "nanoaods_postfit",
                      vector<int> years = {2016,2017,2018},
                      TString out_dir = "paper",
                      bool add_sys = true, bool bdt_vars = false) {

  const bool is_prelim(false); //preliminary plots or not
  useOpenGL_ = 1; // use for rendering

  // Check if this is for the Z'
  const bool zprime = selection == "zpemu";
  if(zprime) {
    out_dir += "_zprime";
    selection = "emu";
    zprime_ = 1;
  }
  if(bdt_vars) out_dir += "_bdt_vars";
  if(is_prelim) out_dir += "_prelim";

  //setup the datacards
  years_         = years;
  hist_dir_      = hist_dir;
  hist_tag_      = "clfv";
  useEmbed_      = (selection == "emu") ? 0 : 2;
  drawStats_     = false; //data and MC total integrals
  doStatsLegend_ = false; //process yields in the legend
  simplified_sys_ = true; //don't do detailed systematics in plotting

  //initialize the plotter
  int status = nanoaod_init(selection.Data(), hist_dir_, out_dir);
  if(status) return status;

  dataplotter_->verbose_ = 0;
  dataplotter_->plot_signal_ = (zprime) ? 0 : 1;
  dataplotter_->misid_label_ = "j#rightarrow#tau_{h}";
  dataplotter_->qcd_label_ = "Nonprompt e#mu";
  dataplotter_->region_label_ = true; // add a label for the data region
  dataplotter_->figure_format_ = "pdf"; //save PDF files
  // dataplotter_->figure_format_ = "eps"; //save EPS files
  // dataplotter_->figure_format_ = "png"; //save PNG files
  dataplotter_->print_root_canvas_ = true; //add root files with the canvases for easier paper updates
  dataplotter_->plot_y_title_ = 1; //add y-axis label
  dataplotter_->add_y_unit_ = 1;
  dataplotter_->cms_is_prelim_ = is_prelim; //not preliminary or not
  const double canvas_scale(1.0);
  dataplotter_->canvas_x_ *= canvas_scale;
  dataplotter_->canvas_y_ *= canvas_scale;
  dataplotter_->signal_fill_style_ = 3004;
  dataplotter_->stat_unc_color_ = kGray + 1;
  dataplotter_->stat_unc_alpha_ = 0.5;
  dataplotter_->stat_unc_style_ = 1001;
  dataplotter_->lum_txt_x_ = 0.00;
  dataplotter_->lum_txt_y_ = 0.932;
  dataplotter_->cms_txt_x_ = 0.00;
  dataplotter_->cms_txt_y_ = 0.93;
  dataplotter_->region_x_  = 0.01;
  dataplotter_->region_y_  = 0.03;
  dataplotter_->x_title_offset_ = 0.85;
  dataplotter_->y_title_offset_ = 0.35;
  dataplotter_->x_label_size_ = 0.12;
  dataplotter_->y_label_size_ = 0.12;
  dataplotter_->x_label_offset_ = 0.015;
  dataplotter_->y_label_offset_ = 0.02;
  dataplotter_->cms_txt_size_ = 0.08;
  dataplotter_->lower_pad_y1_ = 0.;
  dataplotter_->upper_pad_topmargin_ = 0.08;
  dataplotter_->lower_pad_topmargin_ = 0.04;
  dataplotter_->lower_pad_botmargin_ = 0.33;
  dataplotter_->legend_x1_ = 0.03;
  dataplotter_->legend_y1_ = 0.90;
  dataplotter_->legend_x2_ = 0.03;
  dataplotter_->legend_y2_ = 0.73;
  dataplotter_->ratio_plot_min_ = 0.8;
  dataplotter_->ratio_plot_max_ = 1.2;
  TGaxis::SetMaxDigits(3);
  TGaxis::SetExponentOffset(-0.06, 0.008, "Y");

  dataplotter_->misid_color_ = kCMSColor_1;
  dataplotter_->qcd_color_   = kCMSColor_2;

  //print each figure
  if(zprime) {
    status += print_mass(8, add_sys, 70., 590.);
    if(bdt_vars) {
      status += print_mva            (8, add_sys);
      status += print_leppt          (8, add_sys);
      status += print_met            (8, add_sys);
      status += print_mt             (8, add_sys);
      status += print_ptratio        (8, add_sys);
      status += print_lepeta         (8, add_sys);
      status += print_lep_metdeltaphi(8, add_sys);
    }
  } else if(selection == "emu") { //Z->e+mu
    status += print_mass(8, add_sys);
    if(bdt_vars) {
      status += print_mva            (8, add_sys);
      status += print_leppt          (8, add_sys);
      status += print_met            (8, add_sys);
      status += print_mt             (8, add_sys);
      status += print_ptratio        (8, add_sys);
      status += print_lepeta         (8, add_sys);
      status += print_lep_metdeltaphi(8, add_sys);
    }
  } else { //Z->X+tau
    status += print_mass(8, add_sys);
    if(bdt_vars || selection.Contains("mutau")) {
      status += print_collinear_mass(8, add_sys);
    }
    if(bdt_vars || selection == "mutau") {
      status += print_lep_beta(8, add_sys);
    }
    if(bdt_vars) {
      status += print_leppt          (8, add_sys);
      status += print_mt             (8, add_sys);
      status += print_ptratio        (8, add_sys);
      status += print_lepeta         (8, add_sys);
      status += print_lep_metdeltaphi(8, add_sys);
      status += print_deltaphi       (8, add_sys);
      status += print_jet            (8, add_sys);
    }

    dataplotter_->plot_y_title_ = 0; //add y-axis label
    dataplotter_->add_y_unit_ = 0;
    status += print_mva(8, add_sys);
  }
  return status;
}
