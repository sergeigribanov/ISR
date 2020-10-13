#include <cmath>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <TGraph.h>
#include <TFile.h>
#include "ISRSolverTikhonov.hpp"
#include "utils.hpp"
namespace po = boost::program_options;

typedef struct {
  double thsd;
  double alpha_min;
  double alpha_max;
  int alpha_n;
  std::string vcs_name;
  std::string efficiency_name;
  std::string ifname;
  std::string ofname;
  std::string interp;
  std::string solver;
} CmdOptions;

void setOptions(po::options_description* desc, CmdOptions* opts) {
  desc->add_options()
    ("help,h",
     "A simple tool designed in order to find numerical"
     "solution of the Kuraev-Fadin equation.")
    ("thsd,t", po::value<double>(&(opts->thsd)), "Threshold (GeV).")
    ("enable-solution-positivity,p", "Setting positive limits to solution")
    ("disable-solution-norm,f", 
     "Disable solution norm in Tihonov's regularization functional")
    ("disable-solution-derivative-norm,d",
     "Disable solution derivative norm in Tikhonov's regularization functional")
    ("alpha-min,m", po::value<double>(&(opts->alpha_min))->default_value(1.e-9),
     "Minimum value of Thikhonov's regularization parameter.")
  ("alpha-max,x", po::value<double>(&(opts->alpha_max))->default_value(1.0),
     "Maximum value of Thikhonov's regularization parameter.")
    ("alpha-n,n", po::value<int>(&(opts->alpha_n))->default_value(10),
     "Number of steps in Thikhonov's regularization parameter.")
    ("vcs-name,v", po::value<std::string>(&(opts->vcs_name))->default_value("vcs"),
     "Name of the visible cross section graph.")
    ("efficiency-name,e", po::value<std::string>(&(opts->efficiency_name)),
     "TEfficiency object name")
    ( "ifname,i",
      po::value<std::string>(&(opts->ifname))->default_value("vcs.root"),
      "Path to input file.")(
      "ofname,o",
      po::value<std::string>(&(opts->ofname))->default_value("bcs.root"),
      "Path to output file.")("interp,r",
                              po::value<std::string>(&(opts->interp)),
                              "Path to JSON file with interpolation settings.");
}

void help(const po::options_description& desc) {
  std::cout << desc << std::endl;
}

int main(int argc, char* argv[]) {
  po::options_description desc("Allowed options:");
  CmdOptions opts;
  setOptions(&desc, &opts);
  po::variables_map vmap;
  po::store(po::parse_command_line(argc, argv, desc), vmap);
  po::notify(vmap);
  if (vmap.count("help")) {
    help(desc);
    return 0;
  }
  ISRSolverTikhonov solver(opts.ifname, {
      .efficiencyName = opts.efficiency_name,
      .visibleCSGraphName = opts.vcs_name,
      .thresholdEnergy = opts.thsd,
      .energyUnitMeVs = false});
  if (vmap.count("interp")) {
    solver.setInterpSettings(opts.interp);
  }
  if (vmap.count("disable-solution-norm")) {
    solver.disableSolutionNorm2();
  }
  if (vmap.count("disable-solution-derivative-norm")) {
    solver.disableSolutionDerivativeNorm2();
  }
  if (vmap.count("enable-solution-positivity")) {
    solver.enableSolutionPositivity();
  }
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> a;
  std::vector<double> curv;
  x.reserve(opts.alpha_n);
  y.reserve(opts.alpha_n);
  a.reserve(opts.alpha_n);
  curv.reserve(opts.alpha_n);
  double h = std::pow(opts.alpha_max / opts.alpha_min,  1. / (opts.alpha_n - 1));
  for (int i = 0; i < opts.alpha_n; ++i) {
    double alpha = opts.alpha_min * std::pow(h, i);
    std::cout << "[" << i + 1 << "/" << opts.alpha_n << "]" << std::endl;
    std::cout << "alpha = " << alpha << std::endl;
    std::cout << "--------" << std::endl;
    solver.setAlpha(alpha);
    solver.solve();
    x.push_back(std::sqrt(solver.evalEqNorm2()));
    y.push_back(std::sqrt(solver.evalSmoothnessConstraintNorm2()));
    a.push_back(alpha);
    curv.push_back(solver.evalCurvature());
  }
  TGraph lcurve(opts.alpha_n, x.data(), y.data());
  TGraph curvature(opts.alpha_n, a.data(), curv.data());
  auto fl = TFile::Open(opts.ofname.c_str(), "recreate");
  fl->cd();
  lcurve.Write("lcurve");
  curvature.Write("curvature");
  fl->Close();
  delete fl;
  return 0;
}