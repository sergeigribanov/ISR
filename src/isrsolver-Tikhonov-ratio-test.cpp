#include <string>
#include <boost/program_options.hpp>
#include "ISRSolverTikhonov.hpp"
#include "RatioTest.hpp"
namespace po = boost::program_options;

typedef struct {
  double thsd;
  int n;
  double lambda;
  std::string path_to_model;
  std::string name_of_model_bcs;
  std::string name_of_model_vcs;
  std::string vcs_name;
  std::string efficiency_name;
  std::string ifname;
  std::string ofname;
  std::string interp;
} CmdOptions;

void setOptions(po::options_description* desc, CmdOptions* opts) {
  desc->add_options()
      ("help,h", "Help.")
      ("thsd,t", po::value<double>(&(opts->thsd)), "Threshold (GeV).")
      ("num-rnd-draws,n", po::value<int>(&(opts->n))->default_value(100),
       "Number of visible cross section random draws.")
      ("lambda,l", po::value<double>(&(opts->lambda)), "Regularization parameter (lambda).")
      ("use-solution-norm2,s",
       "REGULARIZATOR: lambda*||solution||^2 if enabled, lambda*||d(solution) / dE||^2 otherwise.")
      ("enable-energy-spread,g", "Enable energy spread")
      ("vcs-name,v",
       po::value<std::string>(&(opts->vcs_name))->default_value("vcs"),
       "Name of the visible cross section graph.")
      ("efficiency-name,e", po::value<std::string>(&(opts->efficiency_name)),
       "TEfficiency object name.")
      ("use-model,u", po::value<std::string>(&(opts->path_to_model)),
       "Path to the file with the model Born and visible cross section TGraphErrors (if needed).")
      ("model-bcs-name,b",
       po::value<std::string>(&(opts->name_of_model_bcs))->default_value("bcsSRC"),
       "Name of the model Born cross section TGraphErrors function.")
      ("model-vcs-name,c",
       po::value<std::string>(&(opts->name_of_model_vcs))->default_value("vcsBlured"),
       "Name of the model visible cross section TGraphErrors function")
      ( "ifname,i",
        po::value<std::string>(&(opts->ifname))->default_value("vcs.root"),
        "Path to input file.")
      ("ofname,o",
       po::value<std::string>(&(opts->ofname))->default_value("isrsolver-chi2-distribution.root"),
       "Path to output file.")
      ("interp,r",
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
  if (!vmap.count("lambda")) {
    std::cout << "[!] You should to set regularization parameter lambda." << std::endl;
    return 0;
  }
  if (!vmap.count("use-model")) {
    std::cout << "[!] use-model command option is obligatory." << std::endl;
    return 0;
  }
  ISRSolverTikhonov solver(
      opts.ifname,
      {.efficiencyName = opts.efficiency_name,
       .visibleCSGraphName = opts.vcs_name,
       .thresholdEnergy = opts.thsd});
  solver.setLambda(opts.lambda);
  if (vmap.count("interp")) {
    solver.setRangeInterpSettings(opts.interp);
  }
  if (vmap.count("enable-energy-spread")) {
    solver.enableEnergySpread();
  }
  if (vmap.count("use-solution-norm2")) {
    solver.disableDerivNorm2Regularizator();
  }
  ratioTestModel(&solver,
                 {.n = opts.n,
                  .modelPath = opts.path_to_model,
                  .modelVCSName = opts.name_of_model_vcs,
                  .modelBCSName = opts.name_of_model_bcs,
                  .outputPath = opts.ofname});
  return 0;
}
