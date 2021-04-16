#ifndef __ISRSOLVER_SLAE_HPP__
#define __ISRSOLVER_SLAE_HPP__

#include <TF1.h>
#include "Interpolator.hpp"
#include "BaseISRSolver.hpp"

class ISRSolverSLAE : public BaseISRSolver {
 public:
  ISRSolverSLAE(const std::string& inputPath, const InputOptions& inputOpts);
  ISRSolverSLAE(const ISRSolverSLAE&);
  virtual ~ISRSolverSLAE();
  const Eigen::MatrixXd& getIntegralOperatorMatrix() const;
  const Eigen::MatrixXd& getBornCSCovMatrix() const;
  virtual void solve() override;
  virtual void save(const std::string& outputPath,
                    const OutputOptions& outputOpts) override;
  void setRangeInterpSettings(const std::vector<std::tuple<bool, int, int>>&);
  void setRangeInterpSettings(const std::string&);
  void evalEqMatrix();
  double interpEval(const Eigen::VectorXd&, double) const;
  double evalConditionNumber() const;
  void printConditionNumber() const;

 protected:
  Eigen::MatrixXd& _getIntegralOperatorMatrix();
  Eigen::MatrixXd& _getBornCSCovMatrix();
  Eigen::MatrixXd _energySpreadMatrix() const;
  const Eigen::RowVectorXd& _getDotProdOp() const;
  void _evalDotProductOperator();
  TF1* _createInterpFunction() const;
  TF1* _createDerivativeInterpFunction(std::size_t, const std::string&) const;
  Eigen::VectorXd _bcsErr() const;
  Interpolator _interp;
  bool _isEqMatrixPrepared;

 private:
  Eigen::MatrixXd _integralOperatorMatrix;
  Eigen::MatrixXd _covMatrixBornCS;
  Eigen::RowVectorXd _dotProdOp;
};

#endif
