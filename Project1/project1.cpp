/*
 * project1.cpp
 *
 *  Created on: Jan 31, 2021
 *      Author: erictruong1014
 */


#include <iostream>

using namespace std;

class CSR {
	protected:
		int n; //The number rows of the original matrix
		int m; //The number of columns of the original matrix
		int nonZeros; //The number of non-zeros in the original matrix
		int* values; //value array assuming all are integers
		int* rowPtr; //Array that contains number of non-zero elements in each row of the original matrix
		int* colPos; //Array that contains the column number from the original matrix for the non-zero values
		int valIdx = 0; //next available index of values
		int colIdx = 0; //next available index of colPos
		int rowIdx = 0; //next available index of rowPtr
	public:
		CSR();
		CSR(CSR& matrixB); //copy constructor
		CSR(int rows, int cols, int numNonZeros);
		int getNumRows();
		void addValue(int value); //add a new value in the values array
		void addColumn(int col); //add a column in the colPos array
		void addRow(int row); //add a row in the rowPtr array
		void display(); //print the contents of the three arrays. Each array must be on a different line (separated by a
						//new line character) and you have exactly a single space between each value printed.
		int* matrixVectorMultiply(int* inputVector); //matrix-vector multiplication
		CSR* matrixMultiply(CSR& matrixB); //matrix-matrix multiplication
		int* columnVector(CSR& matrixB, int col); //returns a vector of the desired column of matrixB
		~CSR(); //destructor
};

CSR::CSR() {
	n = 0;
	m = 0;
	values = NULL;
	rowPtr = NULL;
	colPos = NULL;
}

CSR::CSR(CSR& matrixB) {
	n = matrixB.n;
	m = matrixB.m;
	nonZeros = matrixB.nonZeros;
	values = matrixB.values;
	colPos = matrixB.colPos;
	rowPtr = matrixB.rowPtr;
}

CSR::CSR(int rows, int cols, int numNonZeros) {
	n = rows;
	m = cols;
	nonZeros = numNonZeros;
	values = new int[nonZeros];
	colPos = new int[nonZeros];
	rowPtr = new int[n];
}

int CSR::getNumRows() {
	return n;
}

void CSR::addValue(int value) {
	values[valIdx] = value;
	valIdx++;
}

void CSR::addColumn(int col) {
	colPos[colIdx] = col;
	colIdx++;
}

void CSR::addRow(int row) {
	if (row == 0) {
		rowPtr[0] = 0;
	}
	else if (rowIdx != row) {
		rowIdx = row;
		rowPtr[rowIdx] = valIdx - 1;
	}
}

void CSR::display() {
	int currentVal = 0; //current value in values array

	for (int i = 0; i < n; i++) { //current row to print
		for (int j = 0; j < m; j++) { //current column of row
			if (colPos[currentVal] == j) {
				cout << values[currentVal] << " ";
				currentVal++;
			}
			else {
				cout << 0 << " ";
			}
		}
		cout << endl;
	}
}

int* CSR::matrixVectorMultiply(int* inputVector) {
	int* outputVector = new int[n];

	for (int i = 0; i < n; i++) outputVector[i] = 0;

	for (int i = 0; i < n; i++)
		for (int i = 0; i < n; i++)
			for (int j = rowPtr[i]; j < rowPtr[i+1]; j++)
				outputVector[i] = outputVector[i] + values[j] * inputVector[colPos[j]];

	return outputVector;
}

CSR* CSR::matrixMultiply(CSR& matrixB) {
	CSR* outputMatrix;
	int* valsInCol; //values in current column
	int* tempMultVals = new int[n * matrixB.m]; //temp store values from multiplication
	int* tempMultCols = new int[n * matrixB.m]; //temp store column nums of values
	int* tempRowPtr = new int[n]; //temp store rowPtr values
	bool isFirst = true; //whether or not a non-zero value is the first in the row
	int currentVal = 0; //current value of multiplication process
	int nonZeroCount = 0; //number of non-zeros during multiplication

	for (int i = 0; i < n; i++) { //current row of multiplication
		isFirst = true;
		for (int j = 0; j < matrixB.m; j++) { //current column of input matrix
			valsInCol = columnVector(matrixB, j);
			for (int k = rowPtr[i]; k < rowPtr[i+1]; k++) { //iterating through current row
				tempMultVals[currentVal] = tempMultVals[currentVal] + values[k] * valsInCol[colPos[k]];
				tempMultCols[currentVal] = j;
				currentVal++;
				if ((values[k] * valsInCol[colPos[k]]) > 0) { //non-zero result
					if (isFirst) {
						tempRowPtr[i] = nonZeroCount;
					}
					nonZeroCount++;
					isFirst = false;
				}
			}
			delete [] valsInCol;
		}
	}

	outputMatrix = new CSR(n, matrixB.m, nonZeroCount);

	for (int i = 0; i < nonZeroCount; i++) { //copy values from tempMultVals and tempMultCols to output values and colPos
		if (tempMultVals[i] > 0) {
			(*outputMatrix).addValue(tempMultVals[i]);
			(*outputMatrix).addColumn(tempMultCols[i]);
		}
	}

	for (int i = 0; i < n; i++) { //copy values from tempRowPtr to output rowPtr
		(*outputMatrix).rowPtr[i] = tempRowPtr[i];
	}

	return outputMatrix;
}

int* CSR::columnVector(CSR& matrixB, int col) {
	int* vector = new int[matrixB.n];
	bool found = false;

	for (int i = 0; i < matrixB.n; i++) { //current row of column
		found = false;
		for (int j = matrixB.rowPtr[i]; j < matrixB.rowPtr[i+1]; j++) { //each values index of current row
			if (matrixB.colPos[j] == col) { //check if a value in this row is also in column col
				vector[i] = matrixB.values[j];
				found = true;
				break;
			}
		}
		if (!found) { //no non-zero values in this row of the column
			vector[i] = 0;
		}
	}

	return vector;
}

CSR::~CSR() {
	if (values != NULL) delete [] values;
	if (rowPtr != NULL) delete [] rowPtr;
	if (colPos != NULL) delete [] colPos;
	cout << "CSR Object Destroyed!!" << endl;
	n = 0;
	m = 0;
	nonZeros = 0;
}

int main() {

	CSR* A;
	CSR* B;
	int* aVector;
	int numRows, numColumns, numNonZeros;
	int row, col, value;

	//read in the first matrix
	cin >> numRows >> numColumns;
	cin >> numNonZeros;

	A = new CSR(numRows, numColumns, numNonZeros);
	for (int i = 0; i < numNonZeros; i++) {
		cin >> row >> col >> value;
		(*A).addValue(value);
		(*A).addRow(row); //needs to be done cleverly in the method
		(*A).addColumn(col);
	}
	cout << "Matrix A : " << endl;
	(*A).display();

	CSR* C = new CSR(*A); //calling the copy constructor
	cout << "Copied Matrix C: " << endl;
	(*C).display();

	//read in second matrix into B and write code for (*B).display();
	cin >> numRows >> numColumns;
	cin >> numNonZeros;

	B = new CSR(numRows, numColumns, numNonZeros);
	for (int i = 0; i < numNonZeros; i++) {
		cin >> row >> col >> value;
		(*B).addValue(value);
		(*B).addRow(row);
		(*B).addColumn(col);
	}
	cout << "Matrix B: " << endl;
	(*B).display();

	//read in the vector
	cin >> numColumns;
	aVector = new int[numColumns];
	cout << "Vector: " << endl;
	for (int i = 0; i < numColumns; i++) {
		cin >> aVector[i];
		cout << aVector[i] << " ";
	}
	cout << endl;

	//Matrix-Vector multiplication
	int* resultVector = (*A).matrixVectorMultiply(aVector);
	cout << "A*vector: " << endl;
	for (int i = 0; i < (*A).getNumRows(); i++)
		cout << resultVector[i] << " ";
	cout << endl;

	//Matrix-Matrix Multiplication
	CSR* resultMatrix = (*C).matrixMultiply(*B);
	cout << "A*B: " << endl;
	(*resultMatrix).display();

	//Call the destructors
	delete [] aVector;
	delete [] resultVector;
	delete A;
	delete B;
	delete C;
	delete resultMatrix;

	return 0;
}

