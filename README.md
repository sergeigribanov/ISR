
![](figures/badge_license_gpl3.svg)
![](figures/badge_linux_passed.svg)

## Overview

The ISRSolver toolkit is a set of utilities for obtaining the Born cross section using the visible cross section data, as well as a set of utilities for checking the results. The Born cross section can be found using various methods. Some these methods are generally accepted, while other methods, such as the naive method and the Tikhonov regularization method, were first proposed for finding the Born cross section using the visible cross section data in article [] and then implemented in the ISRSolver toolkit.

The utilities are available to the user in a form of executable files that can be run with a set of command line options. The ISRSolver can be also used in a custom C++ or Python project.

## Relationship between Born and visible cross section
Precise measurement of the inclusive cross section of an e+e- annihilation to hadrons is
the main goal of the experiments carried out with the CMD-3 detector at the VEPP-2000 collider. 
This cross section is of interest in connection with the measurement of the fine structure constant 
and the problem of the muon anomalous magnetic moment. The inclusive cross section of an e+e- 
annihilation to hadrons is considered as the sum of the cross sections for exclusive processes 
of e+e- annihilation into different hadronic states. In the experiments carried out with the CMD-3 
detector, the visible cross section <img src="https://render.githubusercontent.com/render/math?math=\large{\sigma_{\rm vis}}">
is measured, while the Born cross section <img src="https://render.githubusercontent.com/render/math?math=\large{\sigma_{\rm Born}}">
is of interest. The visible and Born cross sections are related by the following integral equation:

![equation Kuraev-Fadin](figures/equation1KuraevFadin.png)

where <img src="https://render.githubusercontent.com/render/math?math=\large{\varepsilon(x, s)}"> is a detection efficiency that depends on 
center-of-mass energy <img src="https://render.githubusercontent.com/render/math?math=\large{\sqrt{s}}"> and the energy fraction carried
away by initial state radiation (ISR). Equation (1) were first obtained by E.A. Kuraev amd V.S. Fadin in the work [1].

It should be noted that the center-of-mass energy is known with some accuracy. This accuracy is determined by the spread of particle energies in each beam and the accuracy of energy measurement. Typical standard deviation of the center-of-mass energy in the case of VEPP-2000 is about 1-3 MeV. Thus, instead of equation (1), the relationship between the visible and Born cross sections is given by equation (2):
![equation Kuraev-Fadin](figures/equation2KuraevFadinBlured.png)

where <img src="https://render.githubusercontent.com/render/math?math=\large{\sigma_{E}(s)}"> is the center-of-mass energy standard deviation. Note that for smooth energy dependences of the cross section, the energy spread can be neglected and equation (1) can be used to relate the visible and Born cross sections.

## Methods for obtaining the Born cross section using the visible cross section data 

The most common methods for obtaining the Born cross section using the visible cross section data are the iterative calculation of the radiative correction and the visible cross section fit using qquations (1) or (2), where the Born cross section is specified in some model. 

Equations (1) and (2) can be reduced to a system of linear equations. To reach this goal, it is necessary to interpolate the unknown Born cross section and express the interpolation coefficients in terms of the unknown values of the Born cross section at the energy points where the visible cross section is measured. Then the interpolation of the Born cross section must be substituted into equation (1) or (2). After integration, a system of linear equations for the unknown values of the Born cross section is obtained. This system can be solved directly. This method of finding the Born section is referred to below as the **naive method**. However, it is known that the problem of finding a numerical solution to Fredholm (Volterra) integral equations of the first kind is an ill-posed problem. Equations (1) and (2) are just integral equations of the first kind. Usually, in the case of ill-posed problem, even small perturbations in the right-hand side of the considered equation can lead to large changes in the numerical solution. However, it can be easily shown that in the case of equation (1), the ill-posedness of the problem can be neglected (i.e. equation can be solved with high accuracy using the naive method), since the kernel of the integral operator decreases rapidly with increasing x. In the case of equation (2), the ill-posedness of the problem can be neglected only if the energy spread is much less than the distance between the energy points. Otherwise, to solve equations (1) or (2), it is necessary to use regularization methods, for example, the Tikhonov regularization method. It is known that regularization methods, in contrast to the naive method, give a biased answer and, as a result, an incorrect covariance matrix for a numerical solution. For this reason, regularization methods are not suitable for the obtaining of the Born cross section. Since with a large center-of-mass energy spread, the naive method leads to a large spread of points of the numerical solution, and the regularization methods do not give the correct covariance matrix, then in this case another method for finding the Born cross section should be used, for example, a model fit of the visible cross section using equation (2). More details about the naive method and the Tikhonov regularization method can be found in the article [].

### Naive method

![alt text](figures/equation3KuraevFadinSLE.png)

![alt text](figures/equation4KuraevFadinBluredSLE.png)

### Regularization

### Iterative method

### Visible cross section fit

## Usage

### Naive method

### Regularization

### Iterative method


