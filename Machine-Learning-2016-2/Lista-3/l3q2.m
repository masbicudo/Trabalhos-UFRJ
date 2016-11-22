clear
d1 = dlmread ('data-l3-p-1a.txt', ' ', 10, 0);
d2 = dlmread ('data-l3-p-1b.txt', ' ', 5, 0);

function retval = MLE_uniform(X)
  a = min(X);
  b = max(X);
  retval = 1/(b-a).^length(X);
endfunction

function retval = MLE_normal(X)
  N = length(X);
  mu = sum(X)/N;
  sigma_sq = sum((X - mu).^2)/N;
  retval = ((1/sqrt(sigma_sq*2*pi)).^N)*exp(-N/2);
endfunction

function [val, type] = MLE(X)
  normal = MLE_normal(X);
  uniform = MLE_uniform(X);
  if (normal < uniform)
    val = uniform;
    type = 'uniform';
  else
    val = normal;
    type = 'normal';
  endif
endfunction

mle_d1_normal = MLE_normal(d1);
mle_d1_uniform = MLE_uniform(d1);

mle_d2_normal = MLE_normal(d2);
mle_d2_uniform = MLE_uniform(d2);

[mle_d1, mle_d1_type] = MLE(d1);
[mle_d2, mle_d2_type] = MLE(d2);

figure
plot(d1, 'o')
title(strcat('d1: ', mle_d1_type))

figure
plot(d2, 'o')
title(strcat('d2: ', mle_d2_type))
