# Architecture validation matrix

The first column is the caller and the first row is the callee. `Y` permits a direct include.

|            | hal | sysCore | sysCall | tmLibc | interfaces | services | tasks |
| ---        | --- | ---     | ---     | ---    | ---        | ---      | ---   |
| hal        | Y   | N       | N       | N      | Y          | N        | N     |
| sysCore    | N   | Y       | N       | N      | Y          | Y        | Y     |
| sysCall    | N   | Y       | Y       | N      | Y          | N        | N     |
| tmLibc     | N   | N       | Y       | Y      | Y          | N        | N     |
| interfaces | N   | N       | N       | N      | Y          | N        | N     |
| services   | N   | N       | Y       | Y      | Y          | Y        | N     |
| tasks      | N   | N       | Y       | Y      | N          | Y        | Y     |
