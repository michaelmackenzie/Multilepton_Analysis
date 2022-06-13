# QCD scale factors

QCD scale factors are parameterized in terms of several observables using ABCD methods.
The estimate uses the same-signed (SS) lepton region to estimate the background in the opposite-signed (OS) region,
where transfer factors are estimated in a loose lepton ID region.

The overall measurement works as follows:
- Measure the SS to OS transfer factors in the standard selection but with an anti-isolated muon selection (CR 1)
- Apply the SS to OS to events in CR 1, measure closure corrections for additional observables that are not
well described by using the SS to OS estimate
- Perform the measurement in an anti-isolated muon region while also requiring an
anti-isolated electron (CR 2)
- Estimate the bias introduced by measuring the transfer factors in an anti-isolated muon region by testing
the transfer factors in CR 2 in an anti-isolated electron region with an isolated muon (CR 3)

The total transfer factor is then the product of the CR 1 transfer factor, the CR 1 closure correction, and
the CR 3 bias correction

## Implementation

The SS to OS transfer factors are measured in CR 1 as a function of delta R in jet bins (0, 1, >1). The
2D (electron pT, muon pT) distribution is then corrected in a closure test. The CR 2 and CR 3 SS to OS
transfer factors are both measured as function of the 2D (electron pT, muon pT) distributions, where the
ratio of these two measurements, (CR 3) / (CR 2), is used as the bias correction. To perform this measurement,
the histograms need to be processed twice, once to get the transfer factors, and a second time to get the
closure correction for CR 1.
