#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <err.h>



#define LINES 4
#define INPUTS 3
#define LINES_WTS 3
#define INPUTS_WTS 3
#define IN_FILE "XOR-table.txt"


#define PAT_NUM 4
#define INPUT_NUM 3
#define HIDDEN_NUM 4
#define OUTPUT_NUM 1
#define MIN -0.5
#define MAX 0.5
#define rand() (((double)rand()/((double)RAND_MAX + 1)) * (MAX - MIN)) + MIN


float input_matrix[LINES][INPUTS];
float weight_data[HIDDEN_NUM][INPUT_NUM];
float targets[LINES];


float Targets[PAT_NUM][OUTPUT_NUM];
float hidden[PAT_NUM][HIDDEN_NUM+1];
float deltaO[OUTPUT_NUM];
float weightsIH[INPUT_NUM][HIDDEN_NUM];
float weightsHO[HIDDEN_NUM+1][OUTPUT_NUM];
float output[PAT_NUM][OUTPUT_NUM];

//tools
double sigmoid(double x)
{
    return (1.0/(1.0+exp(-x)));
}

double derivative(double x)
{
    return x*(1.0-x);
}

//back propagation learning algorithm
void bp_learning(int p, float LearnRate, int numInputs,
  int numHidden, int numOutputs)
{
  int i, j;
  float sumDOW[numHidden+1], deltaH[numHidden+1];
  float deltaWeightIH[numInputs][numHidden];
  float deltaWeightHO[numHidden+1][numOutputs];

//i get garbage error when i dont intitialize everything at 0.0
  for (j = 0; j < numOutputs; j++)
  {
    for (i = 0; i < numHidden+1; i++)
    {
      deltaWeightHO[i][j] = 0.0;
    }
  }

  for (j = 0; j < numHidden; j++)
  {
    for (i = 0; i < numInputs; i++)
    {
      deltaWeightIH[i][j] = 0.0;
    }
  }

  for (i = 0; i < numHidden+1; i++)
  {
    sumDOW[i] = 0.0;
  }

for (i = 0; i < numHidden+1; i++)
  {
    deltaH[i] = 0.0;
  }

//back-propagate errors to hidden layer
  for (i = 0; i < numHidden+1; i++)
  {
    for (j = 0; j < numOutputs; j++)
    {
      sumDOW[i] += weightsHO[i][j] * deltaO[j];
    }
  }

  for (i = 1; i < numHidden+1; i++)
  {
    deltaH[i] = sumDOW[i] * derivative(hidden[p][i]);
  }

  for (j = 0; j < numHidden; j++)
  {
    for (i = 0; i < numInputs; i++)
    {
      deltaWeightIH[i][j] = LearnRate * deltaH[j+1] * input_matrix[p][i];
      weightsIH[i][j] += deltaWeightIH[i][j];
    }
  }

  for (j = 0; j < numOutputs; j++)
  {
    for (i = 0; i < numHidden+1; i++)
    {
      deltaWeightHO[i][j] = LearnRate * deltaO[j] * hidden[p][i];
      weightsHO[i][j] += deltaWeightHO[i][j];
    }
  }
}


void simulate_net()
{
  int i, j, p, epoch;
  int numPatterns = PAT_NUM;
  int numInputs = INPUT_NUM;
  int numHidden = HIDDEN_NUM;
  int numOutputs = OUTPUT_NUM;
  float error, LearnRate = 0.7;
  float sumH[numPatterns][numHidden];
  float sumO[numPatterns][numOutputs];
  for (j = 0; j < numHidden; j++)
  {
    for (i = 0; i < numInputs; i++)
    {
    weightsIH[i][j] = rand();
    printf("\nweightsIH[%d][%d]: %f", i, j, weightsIH[i][j]);
    }
  }

  for (j = 0; j < numOutputs; j++)
  {
    for (i = 0; i < numInputs; i++)
    {
      weightsHO[i][j] = rand();
      printf("\nweightsHO[%d][%d]: %f", i, j, weightsHO[i][j]);
    }
  }

  for (j = 0; j < numOutputs; j++)
  {
    for (i = 0; i < numPatterns; i++)
    {
      Targets[i][j] = targets[i];
    }
  }

  for (epoch = 1; epoch <= 10000; epoch++)
  {
    error = 0.0;
    for (p = 0; p < numPatterns; p++)
    {
      for (j = 0; j < numHidden; j++)
      {
        sumH[p][j] = 0.0;
        for (i = 0; i < numInputs; i++)
        {
          sumH[p][j] += input_matrix[p][i] * weightsIH[i][j];
        }
        if (j == 0)
        {
          hidden[p][j] = 1.0;
        }
        hidden[p][j+1] =sigmoid(sumH[p][j]);
      }
      for (j = 0; j < numOutputs; j++)
      {
        sumO[p][j] = 0.0;
        for (i = 0; i < numHidden+1; i++)
        {
          sumO[p][j] += hidden[p][i] * weightsHO[i][j];
        }
        output[p][j] = sigmoid(sumO[p][j]);
        error += MAX * (Targets[p][j] - output[p][j])
        * (Targets[p][j] - output[p][j]);
        deltaO[j] = (Targets[p][j] - output[p][j]) * derivative(output[p][j]);
      }
      bp_learning(p, LearnRate, numInputs, numHidden, numOutputs);
    }
    if (error < 0.009)
    {
      break;
    }
  }
  printf("\nepoch %-5d :  Error = %f", epoch, error);
  printf("\n\n\tResults\n\nPat\t");

  for (i = 1; i < numInputs; i++)
  {
    printf("Input%-4d\t", i);
  }
  for (j = 1; j <= numOutputs; j++)
  {
    printf("Outputs\t\trOutputs\tTarget\t");
  }
  for (p = 0; p < numPatterns; p++)
  {
    printf("\n%d\t", p);
    for (i = 1; i < numInputs; i++)
    {
      printf("%f\t", input_matrix[p][i]);
    }
    for(j = 0; j < numOutputs; j++)
    {
    printf("%f\t~%f\t%f\t", output[p][j], round(output[p][j]), Targets[p][j]);
    }
  }
  printf("\n");
}


void read_input()
{
  FILE *fp;
  fp = fopen(IN_FILE, "r");

  if (fp == NULL)
  {
    errx(1, "Error while opening the inputs data file.");
  }

  else
  {
    int i,j;
    char read_in[150];
    float conv_in;

    fscanf(fp, "%*[^\n]");      //skip first line of text file
    if (LINES == 4)
    {
      for (i = 0; i < LINES; i++)
      {
        for (j = 0; j <= INPUTS; j++)
        {
          //add bias to all patterns
          if (j == 0)
          {
            input_matrix[i][j] = 1;
          }

          // store targets
          else if (j == 3)
          {
            fscanf(fp, "%s", read_in);
            conv_in = atof(read_in);
            targets[i] = conv_in;
          }
          else
          {
            fscanf(fp, "%s", read_in);
            conv_in = atof(read_in);
            input_matrix[i][j] = conv_in;
          }
        }

      }
      fclose(fp);
    }
    else
    {
      printf("Number of patterns do not match that of the text file\n");
    }
  }
}


int main()
{
  read_input();
  simulate_net();
  return 0;
}
