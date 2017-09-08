% The Matlab script we used to automate the QP algorithm.

while true
	pause(1);
	clear;
    fid = fopen('signalToMatlab', 'r');    
    if(fid == -1) continue; end;
    x = fscanf(fid, '%d', 1);
    fclose(fid);

    if x ~= 112233 continue; end;

    delete('signalToMatlab');

	load -ascii H.dat;
	load -ascii f.dat; 
	delete('H.dat');
	delete('f.dat');
%	delete('A.dat');
%	delete('b.dat');

	n = 2000;
	lb = ones(n, 1) - ones(n, 1);
	ub = ones(n, 1);

	tic
	xs = quadprog(H, f, [], [], [], [], lb, ub);
	toc

	save -ascii -double 'matlabOutput' xs;

	fid = fopen('signalFromMatlab', 'w');
	fprintf(fid, '998877');
	fclose(fid);

end

