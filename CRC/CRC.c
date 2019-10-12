#include <stdio.h>
#include <string.h>

char text[100];
char key[100];
char rem[100];

void crc(int sender)
{
	int i,j;
	int keylen,textlen;
	char temp[100];
	strcpy(temp,text);
	keylen=strlen(key);
	if(sender == 1){
		for(i=0;i<keylen-1;i++)
			strcat(temp,"0");
	} else {
		i = keylen - 1;
	}
	textlen=strlen(temp);
	strncpy(rem,temp,keylen);
	while(i!=textlen)
	{
		if(rem[0]=='0')
		{
			strcpy(rem,&rem[1]);
			rem[keylen-1]=temp[++i];
			rem[keylen]='\0';
			continue;
		}
		for(j=0;rem[j]=((rem[j]-'0')^(key[j]-'0'))+'0';j++);
	}
}

int main()
{
	int i=0, j=0;
	int sender = 1;
	int degree, coef, position, choice;
	printf("\n---------------- CRC --------------\n\n");
	printf("Enter the degree of highest degree of message: ");
	scanf("%d", &degree);
	for(i=degree;i>=0;i--){
		printf("Enter coefficient of x^%d: ", i);
		scanf("%d", &coef);
		if(coef>=1){
			text[j] = '1';
		} else {
			text[j] = '0';
		}
		j++;
	}
	text[j] = '\0';
	j = 0;
	printf("Enter the value of highest degree of generator: ");
	scanf("%d", &degree);
	for(i=degree;i>=0;i--){
		printf("Enter coefficient of x^%d: ", i);
		scanf("%d", &coef);
		if(coef>=1){
			key[j] = '1';
		} else {
			key[j] = '0';
		}
		j++;
	}
	key[j] = '\0';
	crc(1);
	printf("\nThe Code word generated: %s\n",strcat(text,rem));
	
	printf("Do you want to transmit the same message or change bits(0/1): ");
	scanf("%d", &choice);
	printf("\n");
	if (choice == 0) {
		crc(0);
		text[strlen(text) - strlen(rem)] = '\0';
		printf("Message Transmitted: %s\n", text);
		for (i = 0; i < strlen(rem); i++){
			if(rem[i] == '1')
				break;
		}
		printf("Syndrome: %s\n", rem);
		if(i == strlen(rem))
			printf("As syndrome is zero Transmission successful\n");
		else
			printf("Logical Error!\n");
	} else {
		label:
		printf("Enter the position to change(1 starting from left): ");
		scanf("%d", &position);
		if(position > strlen(strcat(text, rem))) {
			printf("Wrong Input!\n");
			goto label;
		} else if (position <= strlen(text)) {
			if(text[position - 1] == '0')
				text[position - 1] = '1';
			else
				text[position - 1] = '0';
		} else {
			if(rem[position - strlen(text) - 1] == '0')
				rem[position - strlen(text) - 1] = '1';
			else
				rem[position - strlen(text) - 1] = '0';
		}
		crc(0);
		text[strlen(text) - strlen(rem)] = '\0';
		printf("Message Transmitted: %s\n", text);
		printf("Syndrome: %s\n", rem);
		for (i = 0; i < strlen(rem); i++){
			if(rem[i] == '1')
				break;
		}
		if(i == strlen(rem))
			printf("Logical Error\n");
		else
			printf("As Syndrome is non-zero, Transmission is failed!\n");
	}

	printf("\n");

}
