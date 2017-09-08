#include<vector>
#include<string>
#include<set>
#include<queue>
#include<algorithm>

#include<stdio.h>
#include<cstdlib>

#include<time.h>
#include<math.h>



#define vecInt vector<int>
#define vecDouble vector<double>
#define vecBool vector<bool>
#define matrixInt vector< vecInt >
#define matrixDouble vector< vecDouble >
#define matrixBool vector< vector<bool> >


#define ABS(x) (((x)>0)?(x):-(x))
#define MIN(x, y) (((x)<(y))?(x):(y))
#define MAX(x, y) (((x)<(y))?(y):(x))
#define LSB(x) ( (x) % 2 )
#define SQUARE(x) ((x)*(x))
#define ECHO printf
#define DIFF(timeA, timeB) ( (timeA)-(timeB) )/(double)CLOCKS_PER_SEC

#define FECHO fprintf

#define WAM_fix_denominator

#define PIXEL_CHANGE_CAP 10

#define resizeMatrix(MX, rows, columns) { (MX).resize((rows)); for(int foo_ = 0; foo_ < (rows); foo_++) (MX)[foo_].resize((columns)); }
using namespace std;



struct Pixel { int x, y; };
Pixel newPixel(int x, int y) { Pixel ans; ans.x = x; ans.y = y; return ans; }




struct Change { int x, y, v; };
bool operator <(Change c1, Change c2)
	{
	if(c1.x != c2.x) return c1.x < c2.x;
	else return c1.y < c2.y;
	}
typedef vector<Change> GroupChange;

Change newChange(int x, int y, int v) { Change ans; ans.x = x; ans.y = y; ans.v = v; return ans; }






string toString(int n)
{ 
string ans;
if(n == 0) return string("0");
while(n)
	{
	int d = n % 10;
	n /= 10;
	ans.append(1, d + '0');
	}
reverse(ans.begin(), ans.end());
return ans;
}







