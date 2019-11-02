#include<iostream>
#include<cstdio>
#include<cstring>
#include<algorithm>
#include<string>
#include<queue>
#include<vector>
#include<time.h>
#include<cmath>
using namespace std;
#define INF (1<<30)
typedef long long int ll;
const int MAXN = 100000 + 5;
int wa[2*MAXN], wb[2*MAXN], wv[2*MAXN], WS[2*MAXN];
int cmp(int *r, int a, int b, int l){
    return r[a] == r[b] && r[a + l] == r[b + l];
}
void da(int *r, int *sa, int n, int m){
    int i, j, p, *x = wa, *y = wb, *t;
    for (i = 0; i<m; i++) WS[i] = 0;
    for (i = 0; i<n; i++) WS[x[i] = r[i]]++;
    for (i = 1; i<m; i++) WS[i] += WS[i - 1];
    for (i = n - 1; i >= 0; i--) sa[--WS[x[i]]] = i;
    for (j = 1, p = 1; p<n; j *= 2, m = p)
    {
        for (p = 0, i = n - j; i<n; i++) y[p++] = i;
        for (i = 0; i<n; i++) if (sa[i] >= j) y[p++] = sa[i] - j;
        for (i = 0; i<n; i++) wv[i] = x[y[i]];
        for (i = 0; i<m; i++) WS[i] = 0;
        for (i = 0; i<n; i++) WS[wv[i]]++;
        for (i = 1; i<m; i++) WS[i] += WS[i - 1];
        for (i = n - 1; i >= 0; i--) sa[--WS[wv[i]]] = y[i];
        for (t = x, x = y, y = t, p = 1, x[sa[0]] = 0, i = 1; i<n; i++)
            x[sa[i]] = cmp(y, sa[i - 1], sa[i], j) ? p - 1 : p++;
    }
    return;
}
int Rank[2*MAXN], height[2*MAXN],sa[2*MAXN];
//定义 height[i]=suffix(sa[i-1])和 suffix(sa[i])的最长公共前缀 [2,len]
void calheight(int *r, int *sa, int n){
    int i, j, k = 0;
    for (i = 1; i <= n; i++) Rank[sa[i]] = i;
    for (i = 0; i<n; height[Rank[i++]] = k)
        for (k ? k-- : 0, j = sa[Rank[i] - 1]; r[i + k] == r[j + k]; k++);
    return;
}

char str1[MAXN],str2[MAXN];
int r[2*MAXN];
int cont[2*MAXN];
int st[2*MAXN];

int main()
{
	int k;
	while(scanf("%d",&k)&&k)
	{
		scanf("%s%s",str1,str2);
		int len1=strlen(str1);
		int len2=strlen(str2);
		int len3=0;
		for(int i=0;i<len1;i++) r[len3++]=str1[i];
		r[len3++]='$';
		for(int i=0;i<len2;i++) r[len3++]=str2[i];
		r[len3]=0;
		da(r,sa,len3+1,200);
		calheight(r,sa,len3);
		ll sum=0,ans=0;
		int top=0;
		for(int i=2;i<=len3;i++)
		{
			if(height[i]<k)
			{
				top=0;sum=0;
				continue;
			}
			int num=0;
			while(top && st[top]>height[i])
			{
				sum-=(ll)(st[top]-k+1)*cont[top];
				sum+=(ll)(height[i]-k+1)*cont[top];
				num+=cont[top];
				top--;
			}
			st[++top]=height[i];
			if(sa[i-1]>len1)
			{
				sum+=(ll)(height[i]-k+1);
				cont[top]=num+1;
			}
			else cont[top]=num;
			if(sa[i]<len1) ans+=sum;
		}

		sum=0;
		top=0;
		for(int i=2;i<=len3;i++)
		{
			if(height[i]<k)
			{
				top=0;sum=0;
				continue;
			}
			int num=0;
			while(top && st[top]>height[i])
			{
				sum-=(ll)(st[top]-k+1)*cont[top];
				sum+=(ll)(height[i]-k+1)*cont[top];
				num+=cont[top];
				top--;
			}
			st[++top]=height[i];
			if(sa[i-1]<len1)
			{
				sum+=(ll)(height[i]-k+1);
				cont[top]=num+1;
			}
			else cont[top]=num;
			if(sa[i]>len1) ans+=sum;
		}

		printf("%lld\n",ans);
	}
}
