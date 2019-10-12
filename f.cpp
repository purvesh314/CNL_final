#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <iomanip>
using namespace std;

class Receiver
{
	int bin[25],n;
public:
	Receiver()
{
		for(int i=0;i<25;i++)
			bin[i]=0;
		n=0;
}

	void accept(int *input,int n)
	{
		cout<<"\n\nRECEIVER\n---------\n";
		for(int i=0;i<n;i++)
			bin[i]=input[i];
		this->n=n;
		cout<<"Codeword received: ";
		for(int i=0;i<n;i++)
			cout<<bin[i];
		cout<<endl;
		check();
	}

	void check()
	{
		int d1;
		d1 = 0;
		int min, max = 0, s, j;
		int parity[5],k;
		for(int i=0;i<5;i++)
			parity[i]=0;
		int pos=0;

		for (int i=1;i<=n;i=pow(2,d1))
		{
			++d1;
			k=0;
			j = i;
			s = i;
			min = 1;
			max = i;
			for (; j<=n;)
			{
				for(s=j;max>=min&&s<=n;++min,++s)
				{
					if (bin[s-1] == 1)
						k++;
				}
				j=s+i;
				min=1;
			}
			if (k%2==0)
			{
				parity[pos++]=0;
			}
			else
			{
				parity[pos++]=1;
			}
		}
		int check=0;
		for(int i=0;i<pos;i++)
		{
			check+=parity[i]*pow(2,i);
		}
		cout<<endl;
		if(check==0)
		{
			cout<<"Codeword is correct. No errors detected.\n";
			int d=0;
			int data1[20];
			for(int i=0;i<20;i++)
				data1[i]=0;

			for (int i=1;i<n;++i)
			{
				if ((i)==pow(2,d))
				{
					bin[i-1]=2;
					d++;
				}
			}
			int nn=0;
			for(int i=0;i<n;i++)
			{
				if(bin[i]!=2)
				{
					data1[nn++]=bin[i];
				}
			}

			n=nn;
			cout<<"\nDataword: ";
			for(int i=0;i<n;i++)
				cout<<data1[i];
			cout<<endl;
			int sum,count;
			cout<<"\nData received: ";
			count=0;
			while(count<n)
			{
				sum=0;
				for(int i=0;i<7;i++)
					sum+=data1[count+i]*pow(2,i);
				char op=(char)sum;
				cout<<op;
				count+=7;
			}
		}
		else
		{
			cout<<"There is an error in position: "<<check-1<<endl<<"Correcting the error...";
			correction(check-1);
		}
	}

	void correction(int pos)
	{
		if(bin[pos]==0)
			bin[pos]=1;
		else
			bin[pos]=0;
		cout<<"Corrected codeword: ";
		for(int i=0;i<n;i++)
			cout<<bin[i];
		cout<<"\nCorrections have been made. Checking the codeword again..."<<endl;
		check();
	}
};

class Sender
{
	Receiver send;
	int bin[25],no_bits,no_red;
public:
	Sender()
{
		for(int i=0;i<25;i++)
			bin[i]=0;
		no_bits=0;
		no_red=0;
}

	void accept()
	{
		string in;
		int temp;
		cout<<"SENDER\n----------\nData to be sent: ";
		cin>>in;
		int n=in.length();
		int t;
		int p[10],k;
		for(int i=0;i<n;i++)
		{
			k=0;
			temp=(int)in[i];
			while (temp>0)
			{
				(temp%2)?bin[no_bits]=1:bin[no_bits]=0;
				temp/=2;
				no_bits++;
			}
			cout<<endl;
		}

		while (no_bits+no_red+1>=pow(2,no_red))
			no_red++;
		cout<<"No of data bits: "<<no_bits<<"\nNo of redundant bits: " <<no_red<< "\nTotal Bits :" <<no_bits+no_red<<"\n";
		cout<<"\nDataword: ";
		for(int i=0;i<no_bits;i++)
			cout<<bin[i];
		cout<<endl;
		hamming();
	}

	void hamming()
	{
		int data[no_bits + no_red],d=0,d1=1;
		for(int i=0;i<no_bits+no_red;i++)
			data[i]=0;

		for (int i=1;i<=no_bits+no_red;++i)
		{
			if ((i)==pow(2,d))
			{
				data[i-1]=0;
				++d;
			}
			else
			{
				data[i-1]=bin[d1-1];
				++d1;
			}
		}
		cout<<"\nData Bits Encoded with Parity bits(0): ";
		for (int i=0;i<no_bits + no_red;++i)
			cout<<data[i]<<" ";
		d1 = 0;
		int min, max = 0, parity, s, j;

		for (int i=1;i<=no_bits + no_red;i=pow(2,d1))
		{
			++d1;
			parity = 0;
			j = i;
			s = i;
			min = 1;
			max = i;
			for (; j<=no_bits + no_red;)
			{
				for (s = j; max >= min && s <=no_bits + no_red; ++min, ++s)
				{
					if (data[s-1] == 1)
						parity++;
				}
				j = s + i;
				min = 1;
			}
			if (parity % 2 == 0) // Even Parity
			{
				data[i-1] = 0;
			}
			else
			{
				data[i-1] = 1;
			}
		}
		cout<<endl<<"Codeword: \n";
		for(int i=0;i<no_bits+no_red;++i)
			cout<<setw(3)<<data[i]<<" ";
		cout<<endl<<endl;

		int k1=0,k2=0;
		for(int i=0;i<no_bits+no_red;i++)
		{
			if(i+1==pow(2,k1))
			{
				k1++;
				cout<<setw(3)<<"P"<<k1;
			}
			else
			{
				k2++;
				cout<<setw(3)<<"D"<<k2;
			}
		}

		bool opt;
		cout<<"0.Generate error\n1.Send correct data.\nChoice: ";
		cin>>opt;
		rep:
		if(opt==0)
		{
			cout<<"Which position?\n";
			cin>>d1;
			if(data[d1]==0)
				data[d1]=1;
			else
				data[d1]=0;
		}
		cout<<endl;
		send.accept(data,no_bits+no_red);
	}
};

int main()
{
	Sender user;
	user.accept();
	return 0;
}
