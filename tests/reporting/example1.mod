// Example 1 from Collard's guide to Dynare
var y, c, k, a, h, b;
varexo e, u;

verbatim;
% I want these comments included in
% example1.m
%
var = 1;
end;

parameters beta, rho, alpha, delta, theta, psi, tau;

alpha = 0.36;
rho   = 0.95;
tau   = 0.025;
beta  = 0.99;
delta = 0.025;
psi   = 0;
theta = 2.95;

phi   = 0.1;

model;
c*theta*h^(1+psi)=(1-alpha)*y;
k = beta*(((exp(b)*c)/(exp(b(+1))*c(+1)))
    *(exp(b(+1))*alpha*y(+1)+(1-delta)*k));
y = exp(a)*(k(-1)^alpha)*(h^(1-alpha));
k = exp(b)*(y-c)+(1-delta)*k(-1);
a = rho*a(-1)+tau*b(-1) + e;
b = tau*a(-1)+rho*b(-1) + u;
end;

initval;
y = 1.08068253095672;
c = 0.80359242014163;
h = 0.29175631001732;
k = 11.08360443260358;
a = 0;
b = 0;
e = 0;
u = 0;
end;

shocks;
var e; stderr 0.009;
var u; stderr 0.009;
var e, u = phi*0.009*0.009;
end;

stoch_simul;

shocke = dseries();
shocku = dseries();
@#define endovars=["y", "c", "k", "a", "h", "b"]
@#for var in endovars
  shocke = [shocke dseries(@{var}_e, 2014q3, '@{var}')];
  shocku = [shocku dseries(@{var}_u, 2014q3, '@{var}')];
@#endfor

r = report();
@#for shock in ["e", "u"]
    r = r.addPage('title',{'Dseries/Report Example','Shock @{shock}'},...
                  'titleFormat', {'\Large\bfseries', '\large\bfseries'});
    r = r.addSection('cols', 2);
@#  for var in endovars
      r = r.addGraph('data', shock@{shock}.@{var}, 'title', '@{var}', ...
                     'showGrid', false, 'yTickLabelPrecision', 2, 'yTickLabelZeroFill', false);
      r = r.addSeries('graphHline', 0, 'graphLineColor', 'red');
@#  endfor
    r = r.addVspace('number', 2);
    r = r.addSection('cols', 1);
    r = r.addTable('range', 2022q1:2024q1, 'precision', 5);

@#  for var in endovars
      r = r.addSeries('data', shock@{shock}.@{var});
@#  endfor
@#endfor

r.write();
r.compile();