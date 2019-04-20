n, m = map(int, input().split())
s = list(map(int, input().split()))

def rec(i, f):
	if i == n:
		return f
	return max(rec(i + 1, f * s[i] % m), rec(i + 1, f))

print(rec(0, 1))
