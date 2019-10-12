#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <string.h>
#include <stdio.h>
#include<fstream>
#include<math.h>
#include<string>
#include<cctype>
#include<sstream>
#include<cstring>
using namespace std;

int main(int argc, char const *argv[])
{
	char choice[2],choice1[2];
	int opt,opt1;
	int sock = socket(AF_INET,SOCK_STREAM,0);
	cout<<"Socket created"<<endl;
	struct sockaddr_in server,client;
	ofstream myfile;

	server.sin_family = AF_INET;
	server.sin_port   = htons(8004);
	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock,(struct sockaddr*)&server,sizeof(server))){
		cout<<"\nBIND ERROR\n";
	}
	cout<<"Socket bound"<<endl;
	if (listen(sock,5)<0){
		cout<<"\nERROR ON LISTEN\n";
	}
	cout<<"Server is listening"<<endl;

	socklen_t clientlen = sizeof(client);
	int newsock = accept(sock,(struct sockaddr *)&client,&clientlen);

	float number1,number2,answer;
	char _operator[2],num1[20],num2[20],cont[2],fname[20],content[100],msg_sent[100],msg_recv[100],expression1[20];
	int aud_content[100000],vid_content[100000];
	int numbytes;
	FILE *fp;
	char ans[20];
	string expression;
	char *x;
	string answer_final;

	//------------
	string operators[20];
				  string terms[20];
				    float term_int[20];
				    int term_counter=0;
				    int op_counter=0;
				    float r;
				    int tc=1;
				    float temp;
				    string res;
					stringstream ss;
					char char_ans[20];
	//-------------

	//do{
		recv(newsock,choice,2,0);
		cout<<"Choice received: "<<choice<<endl;
		opt=atoi(choice);
		switch(opt)
		{
		case 1:
			while(true)
			{
				recv(newsock,msg_recv,100,0);
				cout<<"Client: "<<msg_recv<<endl;
				if(msg_recv[0]=='b' && msg_recv[1]=='y' && msg_recv[2]=='e')
					break;
				cout<<"Server: ";
				cin>>msg_sent;
				send(newsock,msg_sent,100,0);
			}
			break;
		case 2:
			recv(newsock,choice1,2,0);
			opt1=atoi(choice1);
			switch(opt1)
			{
			case 1:
				numbytes=recv(newsock,fname,20,0);
				cout<<"\nName of the file: "<<fname<<endl;
				myfile.open(fname);
				recv(newsock,content,100,0);
				myfile.write(content,numbytes);
				cout<<"Text file has been created"<<endl;
				cout<<"Contents of the file: "<<content<<endl;
				break;
			case 2:
				recv(newsock,fname,20,0);
				cout<<"\nName of the file: "<<fname<<endl;
				numbytes=recv(newsock,aud_content,100,0);
				fp=fopen(fname,"w");
				if(fp)
				{
					fwrite(aud_content, numbytes, 1, fp);
					printf("Creating new output file...\n");
				}
				/*
				myfile.open(fname);
				myfile.write(aud_content,numbytes);  */
				cout<<"Audio file has been created"<<endl;
				break;
			case 3:
				recv(newsock,fname,20,0);
				cout<<"\nName of the file: "<<fname<<endl;
				numbytes=recv(newsock,vid_content,100,0);
				fp=fopen(fname,"w");
				if(fp)
				{
					fwrite(vid_content, numbytes, 1, fp);
					printf("Creating new output file...\n");
				}
				/*
				myfile.open(fname);
				myfile.write(vid_content,numbytes);  */
				cout<<"Video file has been created"<<endl;
				break;
			case 4:
				break;
			default:
				cout<<"Incorrect Input"<<endl;
			}
			break;
		case 3:

			recv(newsock,expression1,20,0);
			cout<<"\nExpression : "<<expression1<<endl;

			expression=expression1;



				    for(int i=0; i<expression.length(); i++)
				    {
					    if(expression[i] == '+' || expression[i] == '-' ||
						  expression[i] == '*' || expression[i] == '/')
					    {
						    //extract all operators (in sequential order)
						    operators[op_counter] = expression[i];
						    op_counter++;
						    term_counter++;
					    }
					    else if(isdigit(expression[i]))
					    {
						    //Extract terms into individial 'string' containers
						    terms[term_counter] += expression[i];
					    }
				    }

				    for(int i=0;i<=term_counter;i++)
					cout<<terms[i]<<endl;

				    for(int i=0;i<=op_counter;i++)
					cout<<operators[i]<<endl;

				    for(int i=0;i<=term_counter;i++)
				    {
				    		term_int[i]=float(stoi(terms[i],nullptr,0));
				    		cout<<term_int[i]<<endl;
				    }

				    r=term_int[0];

				    for(int i=0;i<op_counter;i++)
				    {
				    		temp=term_int[tc];
						 if(operators[i]=="+")
						 	r=r+temp;
						 else if(operators[i]=="-")
						 	r=r-temp;
						 else if(operators[i]=="*")
						 	r=r*temp;
						 else if(operators[i]=="/")
						 	r=r/temp;
						 tc++;
						 cout<<r<<endl;
				    }

				    cout<<"Result : "<<r;
				    ss<<r;
				    answer_final=ss.str();
				    strcpy(char_ans,answer_final.c_str());
					send(newsock,char_ans,20,0);

			break;
		case 4:
			close(sock);
			break;
		default:
			cout<<"Incorrect input"<<endl;
		}
	//}while(opt!=4);
}
