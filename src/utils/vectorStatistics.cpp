void calculateMeanAndCovariance(vector<vecDouble > &samples, vecDouble &mean, matrixDouble &cov)
{
int i, j, k;
int N = samples[0].size();

mean.clear();
cov.clear();
mean.resize(N);
resizeMatrix(cov, N, N);
//ECHO("samples.size() = %d\n", samples.size());
for(i = 0; i < N; i++) mean[i] = 0;
for(i = 0; i < N; i++) for(j = 0; j < N; j++) cov[i][j] = 0;

for(i = 0; i < samples.size(); i++)	mean = mean + samples[i];
for(i = 0; i < N; i++) mean[i] /= samples.size();

for(i = 0; i < N; i++)
	for(j = 0; j < N; j++)
		{
		for(k = 0; k < samples.size(); k++)
			{
			cov[i][j] += (samples[k][i] - mean[i])*(samples[k][j] - mean[j]);
//			ECHO("cov[%d][%d] = %lf\n", i, j, cov[i][j]);
			}
		cov[i][j] /= (samples.size() - 1);
//		ECHO("cov[%d][%d] = %lf\n", i, j, cov[i][j]);
		}
}


