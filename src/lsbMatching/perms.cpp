// perms.cpp
// An invokation of an STL routine
// for obtaining a random permutation.

void randomPermutation(int N, vecInt &ans);

void randomPermutation(int N, vecInt &ans)
{
int i, j;
ans.clear();
ans.resize(N);
for(i = 0; i < N; i++) ans[i] = i;
random_shuffle(ans.begin(), ans.end());
}

