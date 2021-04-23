#ifndef __INTERPOLATOR_HPP__
#define __INTERPOLATOR_HPP__
#include <string>
#include <exception>
#include <memory>
#include <nlohmann/json.hpp>
#include "BaseRangeInterpolator.hpp"

using json = nlohmann::json;

typedef struct : std::exception {
  const char* what() const noexcept {
    return "[!] Wrong interpolation ranges.\n";
  }
} InterpRangeException;

class Interpolator {
 public:
  Interpolator();
  Interpolator(const Interpolator&);
  Interpolator(const Eigen::VectorXd&, double);
  Interpolator(const std::vector<std::tuple<bool, int, int>>&,
               const Eigen::VectorXd&, double) noexcept(false);
  Interpolator(const std::string&, const Eigen::VectorXd&, double);
  virtual ~Interpolator();
  double evalKuraevFadinBasisIntegral(int, int, const std::function<double(double, double)>&) const;
  double evalIntegralBasis(int) const;
  double basisEval(int, double) const;
  double basisDerivEval(int, double) const;
  double eval(const Eigen::VectorXd&, double) const;
  double derivEval(const Eigen::VectorXd&, double) const;
  double getMinEnergy() const;
  double getMaxEnergy() const;
  double getMinEnergy(int) const;
  double getMaxEnergy(int) const;
  static std::vector<std::tuple<bool, int, int>>
  loadInterpRangeSettings(const std::string&);
  static std::vector<std::tuple<bool, int, int>>
  defaultInterpRangeSettings(int);
 private:
  static bool checkRangeInterpSettings(const std::vector<std::tuple<bool, int, int>>&,
                                       const Eigen::VectorXd&);
  std::vector<std::shared_ptr<BaseRangeInterpolator>> _rangeInterpolators;
};

#endif
