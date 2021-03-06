#include <iostream>
#include <vector>
#include <functional>
#include <boost/program_options.hpp>
#include <TFile.h>
#include <TF1.h>
#include <TGraph.h>
#include <TEfficiency.h>
#include "KuraevFadin.hpp"
namespace po = boost::program_options;

/**
 * A part of program options
 */
typedef struct {
  /**
   * Number of points in radiative correction graph
   */
  std::size_t n;
  /**
   * Threshold energy
   */
  double thsd;
  /**
   * Minimum center-of-mass energy
   */
  double minen;
  /**
   * Maximum center-of-mass energy
   */
  double maxen;
  /**
   * Path to the input .root file that contains
   * Born cross section function (TF1)
   */
  std::string ifname;
  /**
   * Name of the Born cross section function (TF1)
   */
  std::string bcs_fcn_name;
  /**
   * Name of the detection efficiency object (TEfficiency)
   */
  std::string efficiency_name;
  /**
   * Output file path
   */
  std::string ofname;
} CmdOptions;

/**
 * Setting up program options
 */
void setOptions(po::options_description* desc, CmdOptions* opts) {
  desc->add_options()
      ("help,h", "radiative correction calculator")
      ("thsd,t", po::value<double>(&(opts->thsd)), "threshold (GeV)")
      ("number-of-radocrr-points,n", po::value<std::size_t>(&(opts->n))->default_value(1000),
       "number of radiative correction points")
      ("minen,m",  po::value<double>(&(opts->minen)), "minimum energy")
      ("maxen,x",  po::value<double>(&(opts->maxen)), "maximum energy")

      ( "ifname,i",
        po::value<std::string>(&(opts->ifname))->default_value("input.root"),
        "path to input file")
      ("ofname,o",
       po::value<std::string>(&(opts->ofname))->default_value("output.root"),
       "path to output file")
      ("born-cs-fcn-name,b",
       po::value<std::string>(&(opts->bcs_fcn_name))->default_value("f_bcs"),
       "the name of the Born cross section function (TF1*)")
      ("efficiency-name,e", po::value<std::string>(&(opts->efficiency_name)),
       "name of a detection efficiency object (TEfficiency*)");
}

/**
 * Help message
 */
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
  if (!vmap.count("maxen") ||
      !vmap.count("minen") ||
      !vmap.count("thsd")) {
    std::cout << "[!] You need to set threshold energy, maximum and minimum energy" << std::endl;
    return 0;
  }
  TEfficiency* teff = nullptr;

  auto fl = TFile::Open(opts.ifname.c_str(), "read");
  auto fbcs = dynamic_cast<TF1*>(fl->Get(opts.bcs_fcn_name.c_str())->Clone());
  if (vmap.count("efficiency-name")) {
    teff = dynamic_cast<TEfficiency*>(fl->Get(opts.efficiency_name.c_str())->Clone());
  }
  fl->Close();
  delete fl;
  std::function<double(double)> fcn =
      [fbcs](double energy) {
        double result = fbcs->Eval(energy);
        return result;
      };
  std::function<double(double, double)> eff =
      [teff](double x, double en) {
        int bin = teff->FindFixBin(x, en);
        double result = teff->GetEfficiency(bin);
        return result;
      };
  std::vector<double> ens;
  std::vector<double> radcorrs;
  ens.reserve(opts.n);
  radcorrs.reserve(opts.n);
  const double s_th = opts.thsd * opts.thsd;
  const double eh = (opts.maxen - opts.minen) / opts.n;
  for (std::size_t i = 1; i < opts.n + 1; ++i) {
    double en = opts.minen + i * eh;
    ens.push_back(en);
    if (teff) {
      radcorrs.push_back(convolutionKuraevFadin(en, fcn, 0, 1 - s_th / en / en, eff) / fcn(en) - 1);
    } else {
      radcorrs.push_back(convolutionKuraevFadin(en, fcn, 0, 1 - s_th / en / en) / fcn(en) - 1);
    }
  }
  delete fbcs;
  delete teff;
  TGraph gradcorr(opts.n, ens.data(), radcorrs.data());
  auto ofl = TFile::Open(opts.ofname.c_str(), "recreate");
  ofl->cd();
  gradcorr.Write("radcorr");
  ofl->Close();
  delete ofl;
  return 0;
}
