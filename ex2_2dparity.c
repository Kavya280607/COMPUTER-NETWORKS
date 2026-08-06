#include <stdio.h>
#include <string.h>
#define MAX 100
#define BITS 8
int data[MAX][BITS];
int rows, rowparity[MAX], colparity[BITS];
char message[MAX];
void charToBinary() 
{
   int i,j;
   for(i = 0; i < rows; i++) 
   {
      unsigned char ch = message[i];
      for(j = BITS - 1; j >= 0; j--) 
      {
         data[i][j] = ch % 2;
         ch /= 2;
      }
   }
}
void rowpar()
{
   int i, j;
   for (i = 0; i < rows; i++)
   {
      int count = 0;
      for (j = 0; j < BITS; j++)
      {
         if (data[i][j] == 1)
            count++;
      }
      rowparity[i] = count % 2;
   }
}
void colpar()
{
   int i, j;
   for (j = 0; j < BITS; j++)
   {
      int count = 0;
      for (i = 0; i < rows; i++)
      {
         if (data[i][j] == 1)
            count++;
      }
      colparity[j] = count % 2; 
   }
}
void print2D()
{
   int i, j;
   printf("\n--- 2D Parity Matrix ---\n");
   printf("Char\tData Bits\tRow Parity\n");
   for (i = 0; i < rows; i++)
   {
      printf("%c\t", message[i]);
      for (j = 0; j < BITS; j++)
         printf("%d", data[i][j]);
      printf("\t%d\n", rowparity[i]);
   }  
   printf("Col P\t");
   for (j = 0; j < BITS; j++)
      printf("%d", colparity[j]);
   printf("\n\n");
}
int main()
{
   int i, j;
   FILE *fp;  
   printf("Enter message: ");
   scanf("%[^\n]", message); 
   rows = strlen(message); 
   charToBinary(); 
   rowpar();
   colpar();
   print2D();
   fp = fopen("output.txt", "w");
   if (fp == NULL)
   {
      printf("Cannot create output.txt\n");
      return 1;
   }
   fprintf(fp, "%d\n", rows);
   for (i = 0; i < rows; i++)
   {
      fprintf(fp, "%c", message[i]);
   }
   fprintf(fp, "\n");
   for (i = 0; i < rows; i++)
   {
      for (j = 0; j < BITS; j++)
         fprintf(fp, "%d", data[i][j]);
      fprintf(fp, "%d\n", rowparity[i]);
   }
   for (j = 0; j < BITS; j++)
      fprintf(fp, "%d", colparity[j]);
   fprintf(fp, "\n");
   fclose(fp);
   printf("Data successfully written to output.txt\n");
   return 0;
}
#include <stdio.h>
#include <stdlib.h>
#define MAX 100
#define BITS 8
int data[MAX][BITS];
int rows;
int rec_rowparity[MAX], rec_colparity[BITS]; 
int calc_rowparity[MAX], calc_colparity[BITS]; 
char message[MAX];
void recalculateParity()
{
   int i, j, count;
   for (i = 0; i < rows; i++)
   {
      count = 0;
      for (j = 0; j < BITS; j++)
      {
         if (data[i][j] == 1)
            count++;
      }
      calc_rowparity[i] = count % 2;
   }
   for (j = 0; j < BITS; j++)
   {
      count = 0;
      for (i = 0; i < rows; i++)
      {
         if (data[i][j] == 1)
            count++;
      }
      calc_colparity[j] = count % 2;
   }
}
void printMatrix(const char* title)
{
   printf("\n--- %s ---\n", title);
   printf("Idx\tChar\tData Bits\tCal Row\tRec Row P\n");
   printf("--------------------------------------------------\n");
   
   for (int i = 0; i < rows; i++)
   {
      printf("[%d]\t%c\t", i, message[i]);
      for (int j = 0; j < BITS; j++)
      {
         printf("%d", data[i][j]);
      }
      printf("\t%d\t\t%d\n",calc_rowparity[i],rec_rowparity[i]);
   }  
   printf("---------------------------------------------------\n"); 
   printf("\nCal Col P :\t");
   for (int j = 0; j < BITS; j++) printf("%d", calc_colparity[j]);
   printf("\nRec Col P :\t");
   for (int j = 0; j < BITS; j++) printf("%d", rec_colparity[j]);
   printf("\n\n");
}
int main()
{
   int ch;
   FILE *fp = fopen("output.txt", "r");
   if (fp == NULL)
   {
      printf("Error: Could not open output.txt\n");
      return 1;
   } 
   fscanf(fp, "%d\n", &rows);
   for (int i = 0; i < rows; i++)
   {
      fscanf(fp, "%c", &message[i]);
   }
   message[rows] = '\0'; 
   fscanf(fp, "\n");
   for (int i = 0; i < rows; i++)
   {
      for (int j = 0; j < BITS; j++)
      {
         fscanf(fp, "%1d", &data[i][j]);
      }
      fscanf(fp, "%1d\n", &rec_rowparity[i]);
   }
   for (int j = 0; j < BITS; j++)
   {
      fscanf(fp, "%1d", &rec_colparity[j]);
   }
   fclose(fp);
   printf("Original Message: %s\n", message);
   printf("Number of rows: %d\n", rows);
   recalculateParity();
   printMatrix("Data Matrix (As Received / Unmodified)");
   int target_row, target_col;
   printf("Would you like to modify any bit? (1 for Yes, 0 for No): ");
   scanf("%d", &ch);
   if (ch == 1)
   {
       printf("Enter row index to modify (0 to %d): ", rows - 1);
       scanf("%d", &target_row);
       printf("Enter bit index to modify (0 to %d): ", BITS - 1);
       scanf("%d", &target_col);   
       if (target_row >= 0 && target_row < rows && target_col >= 0 && target_col < BITS)
       {
          data[target_row][target_col] = !data[target_row][target_col];
          printf("\nBit flipped at Row %d, Bit %d.\n", target_row, target_col);
          recalculateParity();
          printMatrix("Data Matrix (After Modified)");
       }
       else
       {
          printf("\nInvalid indices Proceeding with original matrix\n\n");
       }
   }
   else if (ch == 0)
   {
       printf("\nVerification running on unmodified matrix\n\n");
   }
   else
   {
       printf("\nInvalid choice input\n\n");
   }
   int error_detected = 0;
   for (int i = 0; i < rows; i++)
   {
        if (rec_rowparity[i] != calc_rowparity[i])
        {
           error_detected = 1;
           break;
        }
   }  
   if (!error_detected)
   {
       for (int j = 0; j < BITS; j++)
       {
            if (rec_colparity[j] != calc_colparity[j])
            {
               error_detected = 1;
               break;
            }
        }
   }
   printf("--- Parity Check Result ---\n");
   if (error_detected)
   {
      printf("Error Detected: Received parity does NOT match calculated parity\n");
   }
   else
   {
      printf("No Error: Received parity matches calculated parity\n");
   }
   return 0;
}

