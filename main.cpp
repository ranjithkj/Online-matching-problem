//
//  main.cpp
//  Repeated_Matching
//
//  Created by Ranjith Kumar on 2017-03-17.
//  Copyright (c) 2017 Ranjith Kumar. All rights reserved.
//

#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>

using namespace std;

struct solution
{
    int cardinality;
    vector < vector < int > > list_matches;
};
struct seeker
{
    int seeker_id;
    int seeker_type;
    int arrival_time;
    int renege_time;
    vector <int> pref_list;
    int waiting_time;
    int matched = -1;
};
struct donor
{
    int donor_id;
    int donor_type;
    int arrival_time;
    int renege_time;
    int waiting_time;
    int matched = -1;
};

solution Sol;
seeker *seek;
donor *don;
int problem_size,D,S,*pref_size,size,num_seekers,num_donors;
float **L;
vector < vector < int > > preference_list;
vector < int > donor_list;
vector < vector < int > > solution;
vector <int> unmatched_donors;
vector < vector < int > > seekers_arrivals;
vector < vector < int > > donors_arrivals;
vector < float > reneging_functions;
vector < vector < int > > seekers_numbers;
vector < vector < int > > donors_numbers;
vector <int> seeker_queue;
vector <int> donor_queue;




int parameters_assign()
{
    
    seek = new seeker[num_seekers];
    don = new donor[num_donors];
    for(int k = 0; k < num_seekers; k++)
    {
        for( int i = 0; i < seekers_numbers.size(); i++)
        {
            for (int j = 0; j < seekers_numbers[i].size(); j++)
            {
                if( k+1 == seekers_numbers[i][j])
                {
                    seek[k].seeker_type = i+1; cout << "type: " << seek[k].seeker_type << endl;
                    seek[k].arrival_time = seekers_arrivals[i][j]; cout << "arrival time: "<<seek[k].arrival_time << endl;
                    seek[k].renege_time  = seek[k].arrival_time + 1/reneging_functions[i]; cout << "renege time:" << seek[k].renege_time << endl;
                }
            }
        }
    }
    
    for (int k = 0; k < num_donors; k++)
    {
        for( int i = 0; i < donors_numbers.size(); i++)
        {
            for (int j = 0; j < donors_numbers[i].size(); j++)
            {
                if( k+1+num_seekers == donors_numbers[i][j])
                {
                    don[k].donor_type = i+1; cout << "type: " << don[k].donor_type << endl;
                    don[k].arrival_time = donors_arrivals[i][j]; cout << "arrival time: "<<don[k].arrival_time << endl;
                    don[k].renege_time  = don[k].arrival_time + 1/reneging_functions[S + i]; cout << "renege time:" << don[k].renege_time << endl;
                }
            }
        }
    }
    return 1;
}

vector <int> donor_pref(seeker seek, donor don, int j)  //11, 28
{
    
    int added = 0; int lag = -1;
    if (L[don.donor_type][seek.seeker_type] == 0)
    {
        //cout << "unfortunaltely they cannot be matched" <<endl;
        return seek.pref_list;
    }
    
    int flag = -1;
    if (seek.pref_list.size() == 0)
    {
        seek.pref_list.push_back(j);
        //cout << "donor added : "<< seek.pref_list[0] << endl;
    }
    else
    {
        long unsigned int k = seek.pref_list.size();
        
        for (int l = 0; l < donors_numbers.size(); l++)
        {
            for(int m = 0; m < donors_numbers[l].size(); m++)
            {
                if (seek.pref_list[k - 1] == donors_numbers[l][m])
                {
                    flag = l;
                    //cout << endl << "flag value: " << l << endl;
                }
            }
        }
        if (L[flag + 1][seek.seeker_type] < L[don.donor_type][seek.seeker_type])
        {
            //cout << "true: "<< L[flag + 1][seek.seeker_type] << " " << L[don.donor_type][seek.seeker_type] << endl;
            for (int g = k - 1; g > -1; g--)
            {
                for (int l = 0; l < donors_numbers.size(); l++)
                {
                    for(int m = 0; m < donors_numbers[l].size(); m++)
                    {
                        if (seek.pref_list[g] == donors_numbers[l][m])
                        {
                            lag = l;
                        }
                    }
                }
                
                if (L[lag + 1][seek.seeker_type] >= L[don.donor_type][seek.seeker_type])
                {
                    seek.pref_list.insert(seek.pref_list.begin() + g + 1, j);
                    //cout << endl << "HEREEEE: " << endl;
                    added = 1;
                    break;
                }
            }
            if (added == 0)
            {
                //cout << endl << "added here:" << endl;
                seek.pref_list.insert(seek.pref_list.begin(), j);
                added = 1;
            }
        }
        
        else
        {
            //cout << "it seems the second one is smaller" << endl;
            //cout << "true: "<< L[flag + 1][seek.seeker_type] << " " << L[don.donor_type][seek.seeker_type] << endl;
            seek.pref_list.push_back(j);
        }
        
    }
    /*cout << " preference list: "<< endl;
    for (int k = 0; k < seek.pref_list.size(); k++)
    {
        cout << seek.pref_list[k] << " ";
    }*/
    
    
    return seek.pref_list;
    
}

int match(int i)
{
    for (int j = 0; j < seek[seeker_queue[i]-1].pref_list.size(); j++)
    {
        for (int k = 0; k < donor_queue.size(); k++)
        {
            if(donor_queue.size() != 0)
            {
               if (seek[seeker_queue[i]-1].pref_list[j] == donor_queue[k])
              {
                seek[seeker_queue[i]-1].matched = donor_queue[k];
                don[donor_queue[k]-num_seekers-1].matched = seeker_queue[i];
                donor_queue[k] = -1;
                return seek[seeker_queue[i]-1].matched;
              }
            }
        }
    }
    return -1;
}

int max(vector < vector < int > > seekers_arrivals,vector < vector < int > > donor_arrivals)
{
    int T;
    
    T  = seekers_arrivals[0][0];
   
    for(int i = 0; i < num_seekers; i++)
    {
        if (T < seek[i].renege_time)
            {
                T = seek[i].renege_time;
            }
        
    }
    for(int i = 0; i < num_donors; i++)
    {
        if (T < don[i].renege_time)
            {
                T = don[i].renege_time;
                
            }
        
    }
    cout << T;
    //cin.get();
    
    return T;
}
int repeated_matching()
{
    int T = 0;
    int result;
    int c = 0,c2 = 0;
    int change1 = 1,change2 = 1;
    
    //cout << "repeated matching: " << endl << endl << endl;
    int G = max(seekers_arrivals,donors_arrivals);
    while (T <= G-1)
    {
        cout << "@ T = "<< T << endl;
        change1 = 0;
        for (int i = 0; i < num_seekers; i++)
        {
            if (seek[i].arrival_time == T)
            {
                cout << "seeker: "<<i+1 << "enters"<< endl;
                seeker_queue.push_back(i+1);
                change1 = 1;
            }
        }
        
        for (int i = 0; i < num_donors; i++)
        {
            if (don[i].arrival_time == T)
            {
               cout << "donor: "<<i+1+num_seekers << "enters"<< endl;
                donor_queue.push_back(i+1+num_seekers);
                change1 = 1;
            }
        }
        
        /*cout << "DONOR QUEUES" << endl;
        
        for (int l = 0; l < donor_queue.size(); l++)
        {
            cout << donor_queue[l] << " ";
        }
        cout << endl;
        */
        //preference list
     //if( change1 == 1 && change2 == 1)
       {
         for (int i = 0; i < seeker_queue.size(); i++)
        {
            if(seeker_queue[i]!= -1 )
          {
            seek[seeker_queue[i]-1].pref_list.resize(0);
            for (int j = 0; j < donor_queue.size(); j++)
            {
               if(donor_queue[j]!= -1)
               {
                   //cout << "i: " << i << " j: " << j << " " << seeker_queue[i]-1 << " " << donor_queue[j]-1-num_seekers <<endl;
                if ((((seek[seeker_queue[i]-1].arrival_time >= don[donor_queue[j]-1-num_seekers].arrival_time) && (seek[seeker_queue[i]-1].arrival_time <= don[donor_queue[j]-1-num_seekers].renege_time) && (L[don[donor_queue[j]-1-num_seekers].donor_type][seek[seeker_queue[i]-1].seeker_type] != 0)))|| ((don[donor_queue[j]-1-num_seekers].arrival_time >= seek[seeker_queue[i]-1].arrival_time) && (don[donor_queue[j]-1-num_seekers].arrival_time <= seek[seeker_queue[i]-1].renege_time)&& (L[don[donor_queue[j]-1-num_seekers].donor_type][seek[seeker_queue[i]-1].seeker_type] != 0)))
                  {
                     //cout << "the edge between seeker "<< seeker_queue[i] << " and donor "<< donor_queue[j] <<" is formed"<< endl;
                     seek[seeker_queue[i]-1].pref_list = donor_pref(seek[seeker_queue[i]-1],don[donor_queue[j]-1-num_seekers],donor_queue[j]);
                     //cout << endl << seek[seeker_queue[i]-1].arrival_time << " " << seek[seeker_queue[i]-1].renege_time << endl;
                  }
                   
               }
            }
          }
        }
       }
        change2 = 0;
       
        for (int i = 0; i < seeker_queue.size(); i++)
         {
           if( seeker_queue[i]!= -1)
          {
            if (seek[seeker_queue[i]-1].renege_time == T)
           {
               c = 1;
             change2 = 1;
             result = match(i);
              if (result != -1)
             {
                cout << "@T = " << T <<" seeker: "<<seeker_queue[i] << " leaves"<< " with " << result << " :)"<< endl;
                //cout << "its preference list :" << endl;
         
                seek[seeker_queue[i]-1].waiting_time = T - seek[seeker_queue[i]-1].arrival_time;
                don[result-1-num_seekers].waiting_time = T - don[result-1-num_seekers].arrival_time;
         
                cout << endl;
                //cin.get();
              }
             else
           {
              cout << "@T = " << T <<" seeker: "<<seeker_queue[i] << " leaves without a matching :(" << endl;
              cout << "its preference list :" << endl;
              seek[seeker_queue[i]-1].waiting_time = T - seek[seeker_queue[i]-1].arrival_time;
              if (seek[seeker_queue[i]-1].pref_list.size() == 0)
         
              {
                 cout << "no preferred donors" << endl;
              }
              cout << endl;
              seek[seeker_queue[i]-1].matched = -1;
            }
              seeker_queue[i] = -1;
         
            cout << endl;
         //cin.get();
         
         
           }
          }
         }
          if (c == 1)
          {
              c = 0;
              c2++;
          }
        
        
        for (int i = 0; i < donor_queue.size(); i++)
        {
            if(donor_queue[i]!= -1 )
            {
               if (don[donor_queue[i]-1-num_seekers].renege_time == T)
              {
                 for (int j = 0; j < seeker_queue.size(); j++)
                  {
                      if( seeker_queue[j]!= -1)
                      {
                        //result = match(j);
                        change2 = 1;
                        cout << "@T = " << T <<" donor: "<<donor_queue[i] << " leaves :("<< endl;
                        cout << endl;
                        //cin.get();
                        donor_queue[i] = -1;
                      }
                   }
                 }
                   }
              }
        T++;
    }
    
    cout << "frequency: " << c2 << endl;
    //cin.get();
    
    return 1;
}

int loadInstance(int w)

{
    int temp;
    float tem;
    cout << "Started loading the instance... " ;
    // define the variables used in this function
    // open the file
    
    
    string title;
    string str;
    title = "/Users/ranjith/Desktop/Instance_Generator/4donors+6seekers+30/Instance";
    ostringstream convert;
    convert << w;
    str = convert.str();
    cout << title << endl;
    title = title + str + ".txt";
    ifstream file(title);
    
    char trash[100];
    // start loading the data
    
    file >> trash;
    file >> problem_size;
    file >> S;  // number of types of seekers
    file >> D; // number of types of donors
    
    cout << endl;
    cout<<"Number of types of donors : "<< D<<" " << endl << "Number of types of seekers : "<<S<<endl;
    vector <int> seeker_arrival;
    
    
    // Seekers Arrivals
    cout << "seeker arrivals:" << endl;
    for (int i = 0; i < S; i++)
    {
        file >> temp;
        while (temp!= -20)
        {
            seeker_arrival.push_back(temp);
            file >> temp;
        }
        
       /* for(int j = 0; j < seeker_arrival.size(); j++)
        {
            cout << seeker_arrival[j] << " ";
        }*/
        cout << endl;
        seekers_arrivals.push_back(seeker_arrival);
        seeker_arrival.resize(0);
    }
    
    // Donor Arrivals
    
    cout << "donor arrivals:" << endl;
    for (int i = 0; i < D; i++)
    {
        file >> temp;
        while (temp!= -20)
        {
            seeker_arrival.push_back(temp);
            file >> temp;
        }
        
       /* for(int j = 0; j < seeker_arrival.size(); j++)
        {
            cout << seeker_arrival[j] << " ";
        }*/
        cout << endl;
        donors_arrivals.push_back(seeker_arrival);
        seeker_arrival.resize(0);
    }
    
    // Matching Utility Functions
    
    L = new float*[D+1];
    for (int i = 1; i <= D; i++)
    {
        L[i] = new float[S+1];
    }
    for (int i = 1; i <= D; i++)
    {
        for ( int j = 1; j <= S; j++)
        {
            file >> L[i][j];
        }
    }
    
    cout << "L:" << endl;
    /*for (int i = 1; i <= D; i++)
    {
        for ( int j = 1; j <= S; j++)
        {
            cout << L[i][j] << " ";
        }
        cout << endl;
    }*/
    
    // Reneging Functions
    
    for (int i = 0; i < S + D; i++)
    {
        file >> tem;
        reneging_functions.push_back(tem);
    }
    cout << "Reneging Functions: " << endl;
   /* for (int i = 0; i < S + D; i++)
    {
        cout << reneging_functions[i] << " ";
    }*/
    seekers_numbers.resize(seekers_arrivals.size());
    int h = 1;
    for (int i = 0; i < seekers_arrivals.size(); i++)
    {
        seekers_numbers[i].resize(seekers_arrivals[i].size());
        for (int j = 0; j < seekers_arrivals[i].size(); j++)
        {
            seekers_numbers[i][j] = h;
            h++;
        }
    }
    size = h -1;
    num_seekers = h - 1;
    donors_numbers.resize(donors_arrivals.size());
    h = 1;
    for (int i = 0; i < donors_arrivals.size(); i++)
    {
        donors_numbers[i].resize(donors_arrivals[i].size());
        for (int j = 0; j < donors_arrivals[i].size(); j++)
        {
            donors_numbers[i][j] = h + size;
            h++;
        }
    }
    cout << endl << "DONOR NUMBERS: "<< endl;
    /*for (int i = 0; i < donors_numbers.size(); i++)
    {
        for (int j = 0; j < donors_numbers[i].size(); j++)
        {
            cout << donors_numbers[i][j] << " ";
        }
        cout << endl;
    }*/
    cout << endl << "asd:";
    num_donors = h - 1;
    size += h-1;
    cout << endl;
    cout << h << endl;
    cout << "finished!" << endl;
    return(1);
}


int main(int argc, const char * argv[])
{
    ofstream graph_utility ("/Users/ranjith/Desktop/Repeated_Matching/Results/Graph_Utility.txt");
    ofstream graph_waitingtime ("/Users/ranjith/Desktop/Repeated_Matching/Results/Graph_Waitingtime.txt");
    ofstream graph_cardinality ("/Users/ranjith/Desktop/Repeated_Matching/Results/Graph_Cardinality.txt");
    ofstream graph_unmatched ("/Users/ranjith/Desktop/Repeated_Matching/Results/Graph_Unmatched.txt");
    ofstream graph_cardinalratio ("/Users/ranjith/Desktop/Repeated_Matching/Results/Graph_CardinalRatio.txt");
    ofstream graph_utlityrratio ("/Users/ranjith/Desktop/Repeated_Matching/Results/Graph_UtilityRatio.txt");
    ofstream graph_waitingratio ("/Users/ranjith/Desktop/Repeated_Matching/Results/Graph_WaitingRatio.txt");

    for( int k = 592; k < 593; k++)
    {
        //srand(time(0));
        //int k = rand()%1000+1;
        cout << "starting the program for the " <<k<<"st time" << endl;
        string title;
        string str;
        title = "/Users/ranjith/Desktop/Repeated_Matching/Results/Result";
        ostringstream convert;
        float objective = 0,waiting_time = 0; int cardinality = 0;
        loadInstance(k);
        parameters_assign();
        repeated_matching();
        convert << k;
        str = convert.str();
        title = title + str + ".txt";
        ofstream file(title);
       
        string h = "repeated_matching_Instance";
        h = h + str;

        // insert code here...
        cout << "Hello world, Here is the answer for your matching problem:\n" << k;
        
        for (int i = 0; i < num_seekers; i++)
        {
            if (seek[i].matched != -1)
            {
                if(don[seek[i].matched-num_seekers-1].arrival_time - seek[i].arrival_time >= 0)
                    
                {
                    waiting_time +=  don[seek[i].matched-num_seekers-1].arrival_time - seek[i].arrival_time;
                    cardinality++;
                }
                else
                {
                    cardinality++;
                }

            }
            else
                waiting_time += seek[i].renege_time - seek[i].arrival_time;
        }
        file << h << endl;
        file << num_seekers << " " << num_donors << endl;
        for (int i = 0; i < num_seekers; i++)
        {
            file << i+1 << " " << seek[i].matched << endl;
            if(seek[i].matched != -1)
            {
                
                //cout << i+1 << " " << sk[i].matched-num_seekers << endl;
                objective += L[don[seek[i].matched-num_seekers-1].donor_type][seek[i].seeker_type];
            }
            //waiting_time += sk[i].waiting_time;
        }
        file << endl << "Objective: ";
        file << objective;
        graph_utility << objective << endl;
        file << endl << "Waiting time: ";
        file << waiting_time;
        file << endl << "Average Waiting time: ";
        file << waiting_time/num_seekers;
        graph_waitingtime << waiting_time/num_seekers << endl;
        file << endl << "Cardinality: ";
        file << cardinality;
        graph_cardinality << cardinality << endl;
        graph_unmatched << "Unmatched donors: " << endl;
        
        for (int i = 0; i < num_donors; i++)
        {
            if(don[i].matched == -1)
            {
                graph_unmatched << i+num_seekers+1 <<" " << don[i].donor_type << " "<< don[i].arrival_time << " " << don[i].renege_time << endl;
            }
        }
        
        
        if (num_seekers <= num_donors)
        {
            
            
            graph_cardinalratio << (float(cardinality)/float(num_seekers)) << endl;
            graph_utlityrratio << float(objective)/float(num_seekers) << endl;
            graph_waitingratio << float(waiting_time)/float(cardinality) << endl;
            
        }
        else
        {
            graph_cardinalratio << float(cardinality)/float(num_donors) << endl;
            graph_utlityrratio << float(objective)/float(num_donors) << endl;
            graph_waitingratio << float(waiting_time)/float(cardinality) << endl;
            
        }

        
        
        
        graph_unmatched << "Unmatched seekers :" << endl;
        for (int i = 0; i < num_seekers; i++)
        {
            if(seek[i].matched == -1)
            {
                graph_unmatched << i+1 <<" " << seek[i].seeker_type << " " << seek[i].arrival_time << " " << seek[i].renege_time << endl;
            }
        }
        
        cout << "instance no :" << k << endl;
        delete [] seek;
        delete [] don;
        preference_list.resize(0);
        donor_list.resize(0);
        solution.resize(0);
        unmatched_donors.resize(0);
        seekers_arrivals.resize(0);
        donors_arrivals.resize(0);
        reneging_functions.resize(0);
        seekers_numbers.resize(0);
        donors_numbers.resize(0);
        donor_queue.resize(0);
        seeker_queue.resize(0);
        
        
    }
    return 0;
}
