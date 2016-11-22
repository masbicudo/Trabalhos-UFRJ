clear;
d = dlmread ('data-EM.txt', ' ', 5, 0);

dh = zeros(1, length(d));

function [arg_sigma_sq, arg_mu, arg_pi] = M_step(X, Z)
  A = cell(1, 2);
  for i = 1:2
    A{i} = X(find(Z==i));
  end
  N = [length(A{1}) length(A{2})];
  arg_mu = [sum(A{1})/N(1) sum(A{2})/N(2)];
  arg_sigma_sq = [sum((A{1} - arg_mu(1)).^2)/N(1) sum((A{2} - arg_mu(2)).^2)/N(2)];
  arg_pi = N/sum(N);
endfunction

function [Z] = E_step(X, Z, arg_sigma_sq, arg_mu, arg_pi)
  for i = 1:length(Z)
    l0 = arg_pi(1)/sqrt(arg_sigma_sq(1)*2*pi)*exp(-((X(i) - arg_mu(1)).^2)/2/arg_sigma_sq(1));
    l1 = arg_pi(2)/sqrt(arg_sigma_sq(2)*2*pi)*exp(-((X(i) - arg_mu(2)).^2)/2/arg_sigma_sq(2));
    if (l0 < l1)
      Z(i) = 2;
    else
      Z(i) = 1;
    endif
  end
endfunction

% chutando valores
s = [1 2]/20;
m = [1 2]*2;
p = [1 2]/3;

% iterando os passos M e E
for i = 1:4
  dh = E_step(d, dh, s, m, p);
  
  figure();
  dd = [reshape([1:length(d)], 1, 200); reshape(d, 1, 200)];
  hold on;
  plot(dd(1,find(dh==1)), dd(2,find(dh==1)), 'ob');
  plot(dd(1,find(dh==2)), dd(2,find(dh==2)), 'or');
  plot(ones(1, 200)*m(1), 'b');
  plot(ones(1, 200)*m(2), 'r');
  plot(ones(1, 200)*(m(1)+sqrt(s(1))), 'b');
  plot(ones(1, 200)*(m(1)-sqrt(s(1))), 'b');
  plot(ones(1, 200)*(m(2)+sqrt(s(2))), 'r');
  plot(ones(1, 200)*(m(2)-sqrt(s(2))), 'r');
  hold off;
  
  [s, m, p] = M_step(d, dh);
end
