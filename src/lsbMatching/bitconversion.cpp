// bitconversion.cpp
// Two routines to convert between
// bit strings and byte strings.

vecBool messageToBits(string message);
string bitsToMessage(vecBool bits);

/*
Take a bit-string and convert it to
a regular string by grouping bits into
groups of 8. <bits> is assumed to have
a length which is divisible by 8.
*/
string bitsToMessage(vecBool bits)
{
int letters = bits.size() / 8;
int i, j;
string ans = "";

for(i = 0; i < letters; i++)
    {
    int c = 0;
    for(j = 0; j < 8; j++)
        c += bits[8*i+j]*(1 << (7-j));
	c -= 128;
    ans.append(1, (char)c);
    }
return ans;
}


/*
Take a string and break each character
into its bit representation.
*/
vecBool messageToBits(string message)
{
vecBool ans;
int i, j;

for(i = 0; i < message.length(); i++)
    {
    int c = message[i];
	c += 128;

    vecBool tmp;
    for(j = 0; j < 8; j++)
        {
        tmp.push_back(c % 2);
        c /= 2;
        }
    // <tmp> is the reversed bit-representation of <message[i]>.
    // Append it in reverse to <ans>.
    for(j = 7; j >= 0; j--)
        ans.push_back(tmp[j]);
    }

return ans;
}

