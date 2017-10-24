#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdlib.h>


#define parent_num 6    //母群個數
#define data_num 150    //資料數
#define dimension 4     //資料維度
#define group_num 3     //分幾群
#define mutation_num 2  //突變要換的點數量
using namespace std;

vector<vector<float> > Dataset;     //存讀到的資料
vector<vector<int> > Groupset;      //存同一群的index
int parent[parent_num][data_num];   //母群
int choice[parent_num][data_num];   //選擇群
int children[parent_num][data_num];   //子群
float SSE[parent_num];
float group_center[parent_num][group_num][dimension];     //群中心
float crossover_rate=0.9;
float mutation_rate=0.03;
int circulartimes=1000;

void read_file() {//char filename[FILENAME_MAX];
    //cout<<"Enter the file name: ";
    //cin>>filename;
    ifstream in("IrisData.txt");
    string strLine;
    while (getline(in, strLine)) {
        stringstream ss(strLine);
        vector<float> row;
        float data;
        string aa;
        while (getline(ss, aa, ',')) {
            data = atof(aa.c_str());
            row.push_back(data);
        }
        Dataset.push_back(row);
    }
}

void bubbleSortIncrease(float a[],int b[], int len)
{
    int i, j, temp,temp2;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < len - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                temp = a[j];
                temp2 = b[j];
                a[j] = a[j + 1];
                b[j] = b[j + 1];
                a[j + 1] = temp;
                b[j + 1] = temp2;
            }
        }
    }
}

void bubbleSortDecrease(int a[], int len)
{
    int i, j, temp;
    for (i = 0; i < len - 1; i++) {
        for (j = 0; j < len - 1 - i; j++) {
            if (a[j] < a[j + 1]) {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
        }
    }

}

void group_find(int z) {

    for (int i = 0; i < data_num; i++) {
        int min = 9999;

        for (int j = 0; j < group_num; j++) {
            float total = 0;
            float distance = 0;
            for (int k = 0; k < dimension; k++) {
                distance = (Dataset[i][k] - group_center[z][j][k]) * (Dataset[i][k] - group_center[z][j][k]);
                total += distance;
            }
            if (total < min) {
                min = total;
                parent[z][i] = j;
            }
        }
    }

    for (int i = 0; i < group_num; i++) {
        vector<int> row;
        for (int j = 0; j < data_num; j++) {
            if (parent[z][j] == i) {
                row.push_back(j);
            }
        }
        Groupset.push_back(row);
    }

}

void center_find(int arr[parent_num][data_num],int z) {
    for (int k = 0; k < group_num; k++) {
        float total[dimension] = {0};
        int count = 0;
        vector<int> row;
        for (int l = 0; l < data_num; l++) {
            if (arr[z][l] == k) {
                for (int m = 0; m < dimension; m++) {
                    total[m] += Dataset[l][m];
                }
                row.push_back(l);
                count++;
            }
        }
        Groupset.push_back(row);

        for (int n = 0; n < dimension; n++) {
            if(total[n]&&count==0){
                group_center[z][k][n]=0;
            }
            group_center[z][k][n] = (total[n] / count);
        }
    }

}

float distance_cal(float total, int i, int j,int z) {
    float distance = 0;
    for (int k = 0; k < dimension; k++) {
        distance += (Dataset[Groupset[i][j]][k] - group_center[z][i][k]) *
                    (Dataset[Groupset[i][j]][k] - group_center[z][i][k]);
    }
    total += distance;
    return total;
}

void SSE_cal(int z) {
    float total = 0;
    for (int i = 0; i < group_num; i++) {
        for (int j = 0; j < Groupset[i].size(); j++) {
            total = distance_cal(total, i, j, z);
        }
    }
    SSE[z] = total;

}

int parent_choice(const int *fitness, int total) {
    int index;
    int random_number=rand()%total;

    if(random_number>=0&&random_number<fitness[0]){
        index=0;
    }
    else if(random_number>=fitness[0]&&random_number<fitness[0]+fitness[1]){
        index=1;
    }
    else if(random_number>=(fitness[0]+fitness[1])&&random_number<(fitness[0]+fitness[1]+fitness[2])){
        index=2;
    }
    else if(random_number>=(fitness[0]+fitness[1]+fitness[2])&&random_number<(fitness[0]+fitness[1]+fitness[2]+fitness[3])){
        index=3;
    }
    else if(random_number>=(fitness[0]+fitness[1]+fitness[2]+fitness[3])&&random_number<(fitness[0]+fitness[1]+fitness[2]+fitness[3]+fitness[4])){
        index=4;
    }
    else{
        index=5;
    }
    return index;
}

void initial() {
    int index_sort[parent_num]={0,1,2,3,4,5};
    int fitness[parent_num];
    int fitness_sort[parent_num];
    float total=0;
    int fitness_total=0;
    for (int i = 0; i < parent_num; i++) {      //隨機產生母群的解
        for (int j = 0; j < group_num; j++) {
            int group_rand = rand() % data_num;
            for(int k=0;k<dimension;k++){
                group_center[i][j][k] = Dataset[group_rand][k];
            }
        }
    }

    cout<<"parent"<<endl;
    //產生母群的SSE
    for (int z = 0; z < parent_num; z++) {
        group_find(z);
        SSE_cal(z);         //計算SSE
        total+=SSE[z];
        Groupset.clear();

//        for(int a=0;a <data_num;a++){
//            cout<<setw(4)<<a<<"["<<parent[z][a]<<"] ";
//        }
        cout << setprecision(5) << "[" << z << "] SSE=" << SSE[z] << endl;
    }

    for(int i=0;i<parent_num;i++){
        fitness_sort[i]=(SSE[i]/total)*1000;
    }
    bubbleSortIncrease(SSE,index_sort,parent_num);
    bubbleSortDecrease(fitness_sort,parent_num);
    for(int i=0;i<parent_num;i++){
        fitness[index_sort[i]]=fitness_sort[i];
    }
    for(int i=0;i<parent_num;i++){
//        cout<<"["<<i<<"] fitness="<<fitness[i]<<endl;
        fitness_total+=fitness[i];
    }

    for(int i=0;i<parent_num;i++){
        int index = parent_choice(fitness, fitness_total);
        for(int j=0;j<data_num;j++){
            choice[i][j]=parent[index][j];
        }
    }
    /*
    for (int i = 0; i < parent_num; i++) {
        cout<<"Choice "<<i<<endl;
        for (int j = 0; j < data_num; j++) {
            cout<<setw(4)<<j<<"["<<choice[i][j]<<"] ";
        }
    }
    */
}

void reproduction() {
    int index_sort[parent_num]={0,1,2,3,4,5};
    int fitness[parent_num];
    int fitness_sort[parent_num];
    float total=0;
    int fitness_total=0;

//    cout<<"parent"<<endl;
    //產生母群的SSE
    for (int z = 0; z < parent_num; z++) {
        center_find(parent,z);     //群中心
        SSE_cal(z);         //計算SSE
        total+=SSE[z];
        Groupset.clear();

//        for(int a=0;a <data_num;a++){
//            cout<<setw(4)<<a<<"["<<parent[z][a]<<"] ";
//        }
//        cout << setprecision(5) << "[" << z << "] SSE=" << SSE[z] << endl;
    }

    for(int i=0;i<parent_num;i++){
        fitness_sort[i]=(SSE[i]/total)*1000;
    }

    bubbleSortIncrease(SSE,index_sort,parent_num);
    bubbleSortDecrease(fitness_sort,parent_num);

    for(int i=0;i<parent_num;i++){
        fitness[index_sort[i]]=fitness_sort[i];
    }

    for(int i=0;i<parent_num;i++){
//        cout<<"["<<i<<"] fitness="<<fitness[i]<<endl;
        fitness_total+=fitness[i];
    }

    for(int i=0;i<parent_num;i++){
        int index = parent_choice(fitness, fitness_total);
        for(int j=0;j<data_num;j++){
            choice[i][j]=parent[index][j];
        }
    }
    /*
    for (int i = 0; i < parent_num; i++) {
        cout<<"Choice "<<i<<endl;
        for (int j = 0; j < data_num; j++) {
            cout<<setw(4)<<j<<"["<<choice[i][j]<<"] ";
        }
    }
    */
}

void crossover(){
    int temp[data_num];
    for(int i=0;i<parent_num/2;i++) {

        int change_point = rand() % data_num;
        if((double)(rand())/(RAND_MAX + 1.0)<crossover_rate) {
            for (int j = change_point; j < data_num; j++) {
                temp[j] = choice[i * 2][j];
                choice[i * 2][j] = choice[i * 2 + 1][j];
                choice[i * 2 + 1][j] = temp[j];
            }
            for (int k = 0; k < data_num; k++) {
                children[i * 2][k] = choice[i * 2][k];
                children[i * 2 + 1][k] = choice[i * 2 + 1][k];
            }
        }
        else{
            for (int k = 0; k < data_num; k++) {
                children[i * 2][k] = choice[i * 2][k];
                children[i * 2 + 1][k] = choice[i * 2 + 1][k];
            }
        }
    }

//    for (int i = 0; i < parent_num; i++) {
//        cout<<"Children "<<i<<endl;
//        for (int j = 0; j < data_num; j++) {
//            cout<<setw(4)<<j<<"["<<children[0][j]<<"] ";
//        }
//    }

}

void mutation(){
    for(int i=0;i<parent_num;i++){
        for(int j=0;j<mutation_num;j++){
            if((double)(rand())/(RAND_MAX + 1.0)<mutation_rate){
                int rand1=rand() % data_num;
                children[i][rand1]=rand() % group_num;
            }
        }
    }

//    for (int i = 0; i < parent_num; i++) {
//        cout<<"Children "<<i<<endl;
//        for (int j = 0; j < data_num; j++) {
//            cout<<setw(4)<<j<<"["<<children[i][j]<<"] ";
//        }
//    }
}

void reset() {
    for(int j=0; j < parent_num; j++){
        for(int k=0;k<data_num;k++){
            parent[j][k]=children[j][k];
        }
    }
}

int main() {
    float min=9999;
    int min_group[data_num];

    clock_t start, end;
    start = clock();

    srand(time(NULL));
    read_file();

    initial();      //初始化
    crossover();    //交配
    mutation();     //突變

    cout << "children" << endl;
    //產生子群的SSE

//    for (int z = 0; z < parent_num; z++) {
//        for(int a=0;a <data_num;a++){
//            cout<<setw(4)<<a<<"["<<children[z][a]<<"] ";
//        }
//    }
    for (int z = 0; z < parent_num; z++) {
        center_find(children,z);     //群中心
        SSE_cal(z);         //計算SSE
        Groupset.clear();

//          for(int a=0;a <data_num;a++){
//                cout<<setw(4)<<a<<"["<<children[z][a]<<"] ";
//          }
        cout << setprecision(5) << "[" << z << "] SSE=" << SSE[z] << endl;
        if(SSE[z]<min){
            min=SSE[z];
            for(int i=0;i<data_num;i++){
                min_group[i]=children[z][i];
            }
        }
    }
    reset();

    for(int i=0;i<circulartimes;i++) {
        reproduction(); //選擇
        crossover();    //交配
        mutation();     //突變

        cout << "children" << endl;
        //產生子群的SSE
        for (int z = 0; z < parent_num; z++) {
            center_find(children,z);     //群中心
            SSE_cal(z);         //計算SSE
            Groupset.clear();
//          for(int a=0;a <data_num;a++){
//                cout<<setw(4)<<a<<"["<<children[z][a]<<"] ";
//          }
            cout << setprecision(5) << "[" << z << "] SSE=" << SSE[z] << endl;

            if(SSE[z]<min){
                min=SSE[z];
                for(int i=0;i<data_num;i++){
                    min_group[i]=children[z][i];
                }
            }
        }

        if(i!=circulartimes-1) {
            reset();
        }
    }

    cout<<"minimal SSE= "<<min<<endl;
    for(int i=0;i<data_num;i++){
        cout<<setw(4)<<i<<"["<<min_group[i]<<"] ";
    }

    end = clock();
    double diff = end - start; // ms
    printf("Program execute time: %.3f  sec", diff / CLOCKS_PER_SEC);
}



