/* Author:  Huang Daoji
* Date  :  2018-05
*/


/* Filename: skim_sketch.cpp
* implementation of skim_sketch
* not tested.
*/



/* 主要解决的是两个集合的并集的元素个数, 要求给出全集的元素和元素个数
* 给定两个流, 需要流的大小
* 需要生成固定的一些hash函数
* 要求four-wise independent的一个二维数组
*/

// to-do: 没有必要设计三个类出来, 事实上两个流是要共享很多东西的

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

void MurmurHash3_x86_32(const void * key, int len,
	uint32_t seed, void * out)
{
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 4;
	int i;

	uint32_t h1 = seed;

	uint32_t c1 = 0xcc9e2d51;
	uint32_t c2 = 0x1b873593;

	//----------
	// body

	const uint32_t * blocks = (const uint32_t *)(data + nblocks * 4);

	for (i = -nblocks; i; i++)
	{
		uint32_t k1 = getblock(blocks, i);

		k1 *= c1;
		k1 = ROTL32(k1, 15);
		k1 *= c2;

		h1 ^= k1;
		h1 = ROTL32(h1, 13);
		h1 = h1 * 5 + 0xe6546b64;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks * 4);

	uint32_t k1 = 0;

	switch (len & 3)
	{
	case 3: k1 ^= tail[2] << 16;
	case 2: k1 ^= tail[1] << 8;
	case 1: k1 ^= tail[0];
		k1 *= c1; k1 = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len;

	h1 = fmix32(h1);

	*(uint32_t*)out = h1;
}

//-----------------------------------------------------------------------------

void MurmurHash3_x86_128(const void * key, const int len,
	uint32_t seed, void * out)
{
	const uint8_t * data = (const uint8_t*)key;
	const int nblocks = len / 16;
	int i;

	uint32_t h1 = seed;
	uint32_t h2 = seed;
	uint32_t h3 = seed;
	uint32_t h4 = seed;

	uint32_t c1 = 0x239b961b;
	uint32_t c2 = 0xab0e9789;
	uint32_t c3 = 0x38b34ae5;
	uint32_t c4 = 0xa1e38b93;

	//----------
	// body

	const uint32_t * blocks = (const uint32_t *)(data + nblocks * 16);

	for (i = -nblocks; i; i++)
	{
		uint32_t k1 = getblock(blocks, i * 4 + 0);
		uint32_t k2 = getblock(blocks, i * 4 + 1);
		uint32_t k3 = getblock(blocks, i * 4 + 2);
		uint32_t k4 = getblock(blocks, i * 4 + 3);

		k1 *= c1; k1 = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;

		h1 = ROTL32(h1, 19); h1 += h2; h1 = h1 * 5 + 0x561ccd1b;

		k2 *= c2; k2 = ROTL32(k2, 16); k2 *= c3; h2 ^= k2;

		h2 = ROTL32(h2, 17); h2 += h3; h2 = h2 * 5 + 0x0bcaa747;

		k3 *= c3; k3 = ROTL32(k3, 17); k3 *= c4; h3 ^= k3;

		h3 = ROTL32(h3, 15); h3 += h4; h3 = h3 * 5 + 0x96cd1c35;

		k4 *= c4; k4 = ROTL32(k4, 18); k4 *= c1; h4 ^= k4;

		h4 = ROTL32(h4, 13); h4 += h1; h4 = h4 * 5 + 0x32ac3b17;
	}

	//----------
	// tail

	const uint8_t * tail = (const uint8_t*)(data + nblocks * 16);

	uint32_t k1 = 0;
	uint32_t k2 = 0;
	uint32_t k3 = 0;
	uint32_t k4 = 0;

	switch (len & 15)
	{
	case 15: k4 ^= tail[14] << 16;
	case 14: k4 ^= tail[13] << 8;
	case 13: k4 ^= tail[12] << 0;
		k4 *= c4; k4 = ROTL32(k4, 18); k4 *= c1; h4 ^= k4;

	case 12: k3 ^= tail[11] << 24;
	case 11: k3 ^= tail[10] << 16;
	case 10: k3 ^= tail[9] << 8;
	case  9: k3 ^= tail[8] << 0;
		k3 *= c3; k3 = ROTL32(k3, 17); k3 *= c4; h3 ^= k3;

	case  8: k2 ^= tail[7] << 24;
	case  7: k2 ^= tail[6] << 16;
	case  6: k2 ^= tail[5] << 8;
	case  5: k2 ^= tail[4] << 0;
		k2 *= c2; k2 = ROTL32(k2, 16); k2 *= c3; h2 ^= k2;

	case  4: k1 ^= tail[3] << 24;
	case  3: k1 ^= tail[2] << 16;
	case  2: k1 ^= tail[1] << 8;
	case  1: k1 ^= tail[0] << 0;
		k1 *= c1; k1 = ROTL32(k1, 15); k1 *= c2; h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	h1 = fmix32(h1);
	h2 = fmix32(h2);
	h3 = fmix32(h3);
	h4 = fmix32(h4);

	h1 += h2; h1 += h3; h1 += h4;
	h2 += h1; h3 += h1; h4 += h1;

	((uint32_t*)out)[0] = h1;
	((uint32_t*)out)[1] = h2;
	((uint32_t*)out)[2] = h3;
	((uint32_t*)out)[3] = h4;
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

static int inner_product(int* p, int* q, int length) {
	int res = 0;
	for (int i = 0; i < length; i += 1) {
		res += p[i] * q[i];
	}
	return res;
}

static int median(int* p, int length) {
	vector<int> pv(p, p + length);
	// 懒了
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

// 实际实现的时候完全可以不用它, 整数用起来挺舒服的, 有必要用map嘛...?
/*
class domain{
public:
int domain_size;
map<int, T> domain_value;
domain(){
domain_size = 0;
domain_value.clear();
}
domain(int s, map<int, T> d_v){
domain_size = s;
domain_value = map<int, T>(d_v); // or domain_value = d_v;
}
~domain(){
domain_size = 0;
domain_value.clear();
}
};
*/

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
	int* flow_element;
	FLOW(int f_l) {
		flow_length = f_l;
		flow_element = new int[flow_length];
	}
	~FLOW() {
		if (flow_element) {
			delete[] flow_element;
		}
	}
};

class hash_table {
public:
	int m;
	int hashtable[s1][s2]; // s1 * s2, s1个hash函数, s2项
	int** epsilon;// m * s1, 全集有m个元素, 这个数组要求是four-wise independent的
	int* estimate_value;
	int n;
	hash_table() {
		m = n = 0;
	}
	hash_table(domain& d, FLOW& flow) {
		//to-do:这个函数设计似乎有点问题, 最好不要每一次为一个流构造一个hash实例, 返回一个hashtable就好?
		m = d.domain_size;
		n = flow.flow_length;
		epsilon = new int*[m];
		for (int i = 0; i < m; i += 1) {
			epsilon[i] = new int[s1];
			for (int j = 0; j < s1; j += 1) {
				epsilon[i][j] = ((hash_template(i, j) & 1) << 1) - 1;//problem here
			}
		}
		estimate_value = new int[m];
		for (int i = 0; i < flow.flow_length; i += 1) {
			for (int j = 0; j < s1; j += 1) {
				hashtable[j][hash_template(j, flow.flow_element[i])] += epsilon[flow.flow_element[i]][j];
			}
		}
	}
	~hash_table() {
		if (epsilon) {
			for (int i = 0; i < m; i += 1) {
				delete[] epsilon[i];
			}
			delete[] epsilon;
		}
		if (estimate_value) {
			delete[] estimate_value;
		}
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
		set<T > E;
		printf("loop1\n");
		for (int i = 0; i < d.domain_size; i += 1) {
			if (i % 1000 == 0) {
			//	printf("the %d-th element...\n", i);
			}
			T u = d.domain_value(i);
			int* f = new int[s1];
			for (int j = 0; j < s1; j += 1) {
				f[j] = h.hashtable[j][hash_template(j, u)] * h.epsilon[u][j];
			}
			int est_u = median(f, s1);
			if (est_u >= 2 * threhold) {
				h.estimate_value[i] = est_u;
				E.insert(u);
			}
			delete[] f;
		}
		printf("loop2\n");
		for (int i = 0; i < d.domain_size; i += 1) {
			if (i % 1000 == 0) {
			//	printf("the %d-th element...\n", i);
			}
			if (h.estimate_value[d.domain_value(i)] > 0) {
				for (int j = 0; j < s1; j += 1) {
					int q = hash_template(j, d.domain_value(i));
					h.hashtable[j][q] = h.hashtable[j][q] - h.estimate_value[i] * h.epsilon[d.domain_value(i)][j];
				}
			}
		}
		cout << "out skim_dense." << endl;
	}

	long long est_sub_join_size(int* v, hash_table& h, domain& d) {
		cout << "in est_sub_join_size..." << endl;
		int* j = new int[s1];
		for (int p = 0; p < s1; p += 1) {
			j[p] = 0;
			for (int i = 0; i < d.domain_size; i += 1) {
				if (i % 1000 == 0) {
				//	printf("the %d-th element...\n", i);
				}
				if (v[d.domain_value(i)] > 0) {
					int q = hash_template(p, d.domain_value(i));
					j[p] = j[p] + h.hashtable[p][q] * v[d.domain_value(i)] * h.epsilon[d.domain_value(i)][p];
				}
			}
		}
		int res = median(j, s1);
		delete j;
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
		int* j_sss = new int[s1];
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

int main() {
	ifstream in1;
	in1.open("C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project\\skimmed-sketch-test1");
	int f_l1 = 0;
	in1 >> f_l1;
	printf("loading flow1, total %ld elements...", (long long)f_l1);
	FLOW f1 = FLOW(f_l1);
	for (int i = 0; i < f_l1; i += 1) {
		in1 >> f1.flow_element[i];
	}
	printf("\n");
	ifstream in2;
	in2.open("C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project\\skimmed-sketch-test2");
	int f_l2 = 0;
	in2 >> f_l2;
	printf("loading flow2, total %ld elements...", (long long)f_l2);
	FLOW f2 = FLOW(f_l2);
	for (int i = 0; i < f_l2; i += 1) {
		in2 >> f2.flow_element[i];
	}
	printf("\n");
	domain d;
	skim_sketch ss = skim_sketch(f1, f2, d);
	cout << ss.join_size;

	ofstream out;
	out.open("C:\\Users\\AndrewHuang\\Documents\\GitHub\\course_pku\\Algorithm Design and Analysis\\Project\\skimmed_ans");
	out << ss.join_size;

	return 0;
}
