/* Author:  Huang Daoji
* Date  :  2018-05
*/

#include <fstream>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include <stdlib.h>
using namespace std;

#ifdef __GNUC__
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE inline
#endif

static FORCE_INLINE uint32_t rotl32(uint32_t x, int8_t r)
{
	return (x << r) | (x >> (32 - r));
}

static FORCE_INLINE uint64_t rotl64(uint64_t x, int8_t r)
{
	return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

#define getblock(p, i) (p[i])

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

static FORCE_INLINE uint32_t fmix32(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

//----------

static FORCE_INLINE uint64_t fmix64(uint64_t k)
{
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xff51afd7ed558ccd);
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
	k ^= k >> 33;

	return k;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x64_128(const void * key, const int len,
	const uint32_t seed, void * out)
{
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 16;
	int i;

	uint64_t h1 = seed;
	uint64_t h2 = seed;

	uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

	//----------
	// body

	const uint64_t * blocks = (const uint64_t *)(data);

	for (i = 0; i < nblocks; i++)
	{
		uint64_t k1 = getblock(blocks, i * 2 + 0);
		uint64_t k2 = getblock(blocks, i * 2 + 1);

		k1 *= c1; k1 = ROTL64(k1, 31); k1 *= c2; h1 ^= k1;

		h1 = ROTL64(h1, 27); h1 += h2; h1 = h1 * 5 + 0x52dce729;

		k2 *= c2; k2 = ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

		h2 = ROTL64(h2, 31); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks * 16);

	uint64_t k1 = 0;
	uint64_t k2 = 0;

	switch (len & 15)
	{
	case 15: k2 ^= (uint64_t)(tail[14]) << 48;
	case 14: k2 ^= (uint64_t)(tail[13]) << 40;
	case 13: k2 ^= (uint64_t)(tail[12]) << 32;
	case 12: k2 ^= (uint64_t)(tail[11]) << 24;
	case 11: k2 ^= (uint64_t)(tail[10]) << 16;
	case 10: k2 ^= (uint64_t)(tail[9]) << 8;
	case  9: k2 ^= (uint64_t)(tail[8]) << 0;
		k2 *= c2; k2 = ROTL64(k2, 33); k2 *= c1; h2 ^= k2;

	case  8: k1 ^= (uint64_t)(tail[7]) << 56;
	case  7: k1 ^= (uint64_t)(tail[6]) << 48;
	case  6: k1 ^= (uint64_t)(tail[5]) << 40;
	case  5: k1 ^= (uint64_t)(tail[4]) << 32;
	case  4: k1 ^= (uint64_t)(tail[3]) << 24;
	case  3: k1 ^= (uint64_t)(tail[2]) << 16;
	case  2: k1 ^= (uint64_t)(tail[1]) << 8;
	case  1: k1 ^= (uint64_t)(tail[0]) << 0;
		k1 *= c1; k1 = ROTL64(k1, 31); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len; h2 ^= len;

	h1 += h2;
	h2 += h1;

	h1 = fmix64(h1);
	h2 = fmix64(h2);

	h1 += h2;
	h2 += h1;

	((uint64_t*)out)[0] = h1;
	((uint64_t*)out)[1] = h2;
}

//-----------------------------------------------------------------------------


#define T int
#define s1 64 // hash函数个数
#define s2 128// 桶的个数

static int inner_product(vector<int> & p, vector<int> & q, int length) {
	int res = 0;
	for (int i = 0; i < length; i += 1) {
		res += p[i] * q[i];
	}
	return res;
}

static int median(vector<int> & p, int length) {
	vector<int> pv(p);
	nth_element(pv.begin(), pv.begin() + pv.size() / 2, pv.end());
	return pv[length / 2];
}


int hash_template(int seed, int n) {
	uint64_t *res = (uint64_t* )malloc(sizeof(uint64_t) * 2);
	MurmurHash3_x64_128(&n, 1, seed, res);

	int ans = (*res) & (s2 - 1);
	free(res);
	return ans;
}


/* only for test */

class domain {
public:
	int domain_size;
	domain() {
		domain_size = 1000000;
	}
	int domain_value(int n) {
		return n;
	}
};

class FLOW {
public:
	int flow_length;
	vector<int> flow_element;
	FLOW(int f_l) {
		flow_length = f_l;
		flow_element.resize(f_l);
	}
	~FLOW() {
	}
};

class hash_table {
public:
	int m;
	int hashtable[s1][s2]; // s1 * s2, s1个hash函数, s2项
	vector<vector<int> > epsilon;// m * s1, 全集有m个元素, 这个数组要求是four-wise independent的
	vector<int> estimate_value;
	int n;
	hash_table() {
		m = n = 0;
	}
	hash_table(domain& d, FLOW& flow) {
		cout << "in hash_table()" << endl;
		m = d.domain_size;
		n = flow.flow_length;
		epsilon.resize(m);
		for (int i = 0; i < m; i += 1) {
			epsilon[i].resize(s1);
			for (int j = 0; j < s1; j += 1) {
				epsilon[i][j] = ((hash_template(i, j) & 1) << 1) - 1;
			}
		}
		estimate_value.resize(m);
		for (int i = 0; i < flow.flow_length; i += 1) {
			for (int j = 0; j < s1; j += 1) {
				hashtable[j][hash_template(j, flow.flow_element[i])] += epsilon[flow.flow_element[i]][j];
			}
		}
	}
	~hash_table() {
	}
};

class skim_sketch {
public:
	long long join_size;
	hash_table h1;
	hash_table h2;
	skim_sketch() {
		join_size = 0;
	}
	skim_sketch(FLOW& f1, FLOW& f2, domain& d) {
		cout << "in skim_sketch()" << endl;
		hash_table h1 = hash_table(d, f1);
		hash_table h2 = hash_table(d, f2);
		join_size = est_skim_join_size(h1, h2, d);
	}
	void skim_dense(hash_table& h, domain& d) {
		cout << "in skim_dense..." << endl;
		int threhold = (h.n / s1) * 0.5;
		for (int i = 0; i < d.domain_size; i += 1) {
			h.estimate_value[i] = 0;
		}
		for (int i = 0; i < d.domain_size; i += 1) {
			T u = d.domain_value(i);
			vector<int> f(s1);
			for (int j = 0; j < s1; j += 1) {
				f[j] = h.hashtable[j][hash_template(j, u)] * h.epsilon[u][j];
			}
			int est_u = median(f, s1);
			if (est_u >= 2 * threhold) {
				h.estimate_value[i] = est_u;
			}
		}
		for (int i = 0; i < d.domain_size; i += 1) {
			if (h.estimate_value[d.domain_value(i)] > 0) {
				for (int j = 0; j < s1; j += 1) {
					int q = hash_template(j, d.domain_value(i));
					h.hashtable[j][q] = h.hashtable[j][q] - h.estimate_value[i] * h.epsilon[d.domain_value(i)][j];
				}
			}
		}
		cout << "out skim_dense." << endl;
	}

	long long est_sub_join_size(vector<int> & v, hash_table& h, domain& d) {
		cout << "in est_sub_join_size..." << endl;
		vector<int> j(s1);
		for (int p = 0; p < s1; p += 1) {
			j[p] = 0;
			for (int i = 0; i < d.domain_size; i += 1) {
				if (v[d.domain_value(i)] > 0) {
					int q = hash_template(p, d.domain_value(i));
					j[p] = j[p] + h.hashtable[p][q] * v[d.domain_value(i)] * h.epsilon[d.domain_value(i)][p];
				}
			}
		}
		int res = median(j, s1);
		cout << "out est_sub_join_size:" << res << endl;
		return res;
	}

	int est_skim_join_size(hash_table& h1, hash_table& h2, domain& d) {
		cout << "in est_skim_join_size..." << endl;
		skim_dense(h1, d);
		skim_dense(h2, d);
		int j_dd = inner_product(h1.estimate_value, h2.estimate_value, d.domain_size);
		int j_ds = est_sub_join_size(h1.estimate_value, h2, d);
		int j_sd = est_sub_join_size(h2.estimate_value, h1, d);
		vector<int> j_sss(s1);
		for (int p = 0; p < s1; p += 1) {
			j_sss[p] = 0;
			for (int q = 0; q < s2; q += 1) {
				j_sss[p] += h1.hashtable[p][q] * h2.hashtable[p][q];
			}
		}
		int j_ss = median(j_sss, s1);
		cout << "out est_skim_join_size." << endl;
		return j_dd + j_ds + j_sd + j_ss;
	}
};

vector<int> cnt1(1000000);
vector<int> cnt2(1000000);

int main() {
	int flow_size = 100000;
	FLOW f1 = FLOW(flow_size);
	for(int i = 0; i < flow_size; i += 1){
		int t = rand() % 1000000;
		f1.flow_element[i] = t;
		cnt1[t] += 1;
	}

	FLOW f2 = FLOW(flow_size);
	for(int i = 0; i < flow_size; i += 1){
		int t = rand() % 1000000;
		f2.flow_element[i] = t;
		cnt2[t] += 1;
	}
	cout << "end" << endl;

	domain d;

	skim_sketch ss(f1, f2, d);

	cout << ss.join_size << endl;

	int exact_ans = 0;
	for(int i = 0; i < flow_size; i += 1){
		exact_ans += cnt1[i] * cnt2[i];
	}
	cout << exact_ans << endl;

	return 0;
}
