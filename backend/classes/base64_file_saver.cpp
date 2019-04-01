#include <fstream>
#include <unordered_map>

using namespace std;

class Base64FileSaver {

private:

	unordered_map<char, int> decoding_table;

public:

	Base64FileSaver() {
		int i = 0;
		for (char c = 'A'; c <= 'Z'; ++c) decoding_table[c] = i++;
		for (char c = 'a'; c <= 'z'; ++c) decoding_table[c] = i++;
		for (char c = '0'; c <= '9'; ++c) decoding_table[c] = i++;
		decoding_table['+'] = i++;
		decoding_table['/'] = i++;
		decoding_table['='] = 0;
	}

	bool save(string &data, string filename) {
	    if (data.length() % 4 != 0)
	    	return false;

	    for (int i = 0; i < data.length(); ++i)
	    	if (decoding_table.count(data[i]) == 0)
	    		return false;

	    ofstream out(filename, ios::binary);

	    unsigned int triple = 0;
	    for (int i = 0; i < data.length(); ++i) {
	    	triple = (triple << 6) + decoding_table[data[i]];
	    	if ((i + 1) % 4)
	    		continue;
	    	for (int j = 2; j >= 0; --j)
	    		if (data[i - j] != '=')
	    			out << (unsigned char)((triple >> j * 8) & 0xFF);
	    	triple = 0;
	    }

	    out.close();

	    return true;
	}
};
