#include <stdio.h>
#include <math.h>

int createDouble(signed long value, signed long mantissa, signed long exponent, unsigned int mantissaLength, unsigned int signValue, unsigned int signExp, double *out);
int chartoInt(char character, signed long *out);
int kahan_append(double *sum, double new, double *cache);

int a = 0;//used for caching input
int state = 0; //state of automat
signed long val = 0; //characteristic
signed long mantissa=0; //Mantissa 
unsigned int mantissaLength=0; //Length of mantissa
signed long exponent=0; //Exponent
unsigned int plusminus = 0; //1->'-'  0->'+' sign of characteristic
unsigned int plusminusExp = 0; //sign of exponent
double sum = 0;
double kahan_cache = 0;
double kahan_sum = 0;
signed long tmp = 0; //temporarily used variable for calculations
double restmp=0;


int main(){
        while(state !=-1){
        a = getchar();
        if(a==10 || a==13 || a==23 || a==12 || a==3 || a==4 || a==EOF){ //new line
            state=-1;
            
            createDouble(val, mantissa, exponent, mantissaLength, plusminus, plusminusExp, &restmp); //building double to tmp
            sum +=restmp;
            kahan_append(&kahan_sum, restmp, &kahan_cache);
            printf("Regular sum: %.17lg\n",sum);
            printf("Kahan_sum:   %.17lg\n",kahan_sum);
            printf("Difference:  %lg\n",fabs(sum-kahan_sum));

            sum=0; kahan_sum=0; kahan_cache=0;


            val=0; mantissa=0; exponent=0; mantissaLength=0; plusminus=0; plusminusExp=0; restmp=0; //resetting all caching variables 
            return 0; // a = getchar(); 

        }

        if(!((a>47&&a<57)||a==43||a==46||a==45||a==101||a==69)) state=0;
        switch(state){
            case 0:
                
                createDouble(val, mantissa, exponent, mantissaLength, plusminus, plusminusExp, &restmp); //building double to tmp

                sum +=restmp;
                kahan_append(&kahan_sum, restmp, &kahan_cache);

                val=0; mantissa=0; exponent=0; mantissaLength=0; plusminus=0; plusminusExp=0; restmp=0; //resetting all caching variables 
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        state = 2;
                        chartoInt(a,&val);
                        break;
                    case '+':
                        state = 1;
                        plusminus = 0; 
                        break;
                    case '-':
                        state = 1;
                        plusminus = 1;
                        break;
                    case '.':
                        state = 3;
                        break;
                }break;


            case 1:
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        state = 2;
                        chartoInt(a,&val);
                        break;
                    case '.':
                        state = 3;
                        break;
                    case '+': case '-':
                        ungetc(a,stdin);
                        state = 0;
                        break;
                }break;


            case 2:
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        chartoInt(a,&tmp);
                        val = val*10+tmp;
                        break;
                    case 'e': case 'E':
                        state = 5;
                        break;
                    case '.':
                        state = 4;
                        break;
                    case '+': case '-':
                        ungetc(a,stdin);
                        state = 0;
                        break;

                }break;


            case 3:
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        state = 4;
                        chartoInt(a,&mantissa);
                        mantissaLength++;
                        break;
                    case '.': case '+': case '-':
                        ungetc(a,stdin);
                        state = 0;
                        break;
                }break;

            case 4:
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
                        chartoInt(a, &tmp);
                        mantissa = 10*mantissa+tmp;
                        mantissaLength++;
                        break;
                    case 'E': case 'e':
                        state = 5;
                        break;
                    case '.': case '+': case '-':
                        ungetc(a,stdin);
                        state = 0;
                        break;
                }break;


            case 5:
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        state = 7;
                        chartoInt(a,&exponent);
                        break;
                    case '+':
                        state = 6;
                        plusminusExp = 0;
                        break;
                    case '-':
                        state = 6;
                        plusminusExp = 1;
                        break;
                    case '.':
                        ungetc(a,stdin);
                        state = 0;
                        break;
                    }break;


            case 6:
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
                        state = 7;
                        chartoInt(a,&exponent);
                        break;
                    case '.': case '+': case '-':
                        ungetc(a,stdin);
                        state = 0;
                        break;
                    }break;


            case 7:
                switch(a){
                    case '0': case '1':case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
                        chartoInt(a,&tmp);
                        exponent = 10*exponent+tmp;
                        break;
                    case '.': case '+': case '-':
                        ungetc(a,stdin);
                        state = 0;
                        break;
                    }break;

        }
    }
    return 0;
}

int chartoInt(char c, signed long *p){
    int tmp = c-48;
    *p = (tmp>=0 && tmp<=9) ? tmp : -127; //ASCII Code Conversion
    return 0;
}
    
//signValue, signExponent: 0->'+' and 1->'-'

int createDouble(signed long value, signed long mantissa, signed long exponent, unsigned int mantissaLength, unsigned int signValue, unsigned int signExp, double *out){
    if(signValue) value = ~value+1;
    if(signExp) exponent = ~exponent+1;
    *out += value;
    *out += (1/pow(10,mantissaLength))*mantissa;
    *out = *out*pow(10,(exponent));

    return 0;

}


int kahan_append(double *sum, double new, double *cache){
    new -= *cache;
    double primitiveSum = *sum+new;
    *cache = (primitiveSum - *sum) - new;
    *sum = primitiveSum;
    return 0;
}
