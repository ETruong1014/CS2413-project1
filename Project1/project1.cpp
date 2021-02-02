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
		int* getColumnVector(int col); //all rows of column col
		int* getRowVec(int row); //all values of given row
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
	values = new int[nonZeros];
	for (int i = 0; i < nonZeros; i++) {
		values[i] = matrixB.values[i];
	}
	colPos = new int[nonZeros];
	for (int i = 0; i < nonZeros; i++) {
		colPos[i] = matrixB.colPos[i];
	}
	rowPtr = new int[n];
	for (int i = 0; i < n; i++) {
		rowPtr[i] = matrixB.rowPtr[i];
	}
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
	int* rowVector = new int[n]; //row as a vector

	if (rowPtr[n - 1] == 0) { //last row is empty
		rowPtr[n - 1] = nonZeros;
	}

	for (int i = 0; i < n; i++) { //current row to print
		rowVector = getRowVec(i);
		for (int j = 0; j < m; j++) {
			cout << rowVector[j] << " ";
		}
		cout << endl;
	}

	cout << "rowPtr: ";
	for (int i = 0; i < n; i++) {
		cout << rowPtr[i] << " ";
	}
	cout << endl;

	cout << "colPos: ";
	for (int i = 0; i < nonZeros; i++) {
		cout << colPos[i] << " ";
	}
	cout << endl;

	cout << "values: ";
	for (int i = 0; i < nonZeros; i++) {
		cout << values[i] << " ";
	}
	cout << endl;
}

int* CSR::matrixVectorMultiply(int* inputVector) {
	int* outputVector = new int[n];

	for (int i = 0; i < n; i++)
		outputVector[i] = 0;

	int sum = 0;
	int start, end;
	for (int i = 0; i < n; i++) { //current row of matrix
		sum = 0;
		start = rowPtr[i];
		if ((i + 1) == n) //if on last row
			end = nonZeros;
		else
			end = rowPtr[i+1];
		for (int j = start; j < end; j++) {
			sum = sum + (values[j] * inputVector[colPos[j]]);
		}
		outputVector[i] = sum;
	}

	return outputVector;
}

CSR* CSR::matrixMultiply(CSR& matrixB) {
	CSR* outputMatrix;
	int* valsInRow; //values in current row
	int* valsInCol; //values in current column
	int* tempMultVals = new int[n * matrixB.m]; //temp store values from multiplication
	int* tempMultCols = new int[n * matrixB.m]; //temp store column nums of values
	int* tempRowPtr = new int[n]; //temp store rowPtr values
	bool isFirst; //whether or not a non-zero value is the first in the row
	int currentVal = 0; //current value of multiplication process
	int nonZeroCount = 0; //number of non-zeros during multiplication

	for (int i = 0; i < n; i++) { //current row of this matrix
		valsInRow = getRowVec(i);
		isFirst = true;
		for (int j = 0; j < matrixB.m; j++) { //current column of matrixB
			valsInCol = matrixB.getColumnVector(j);
			for (int k = 0; k < n; k++) { //multiplying vectors
				tempMultVals[currentVal] = tempMultVals[currentVal] + (valsInRow[k] * valsInCol[k]);
			}
			tempMultCols[currentVal] = j;
			if (tempMultVals[currentVal] > 0) { //non-zero value
				if (isFirst) { //check if this non-zero value is the first in the row
					tempRowPtr[i] = nonZeroCount;
				}
				nonZeroCount++;
				isFirst = false;
			}
			currentVal++;
		}
	}

	outputMatrix = new CSR(n, matrixB.m, nonZeroCount);

	for (int i = 0; i < currentVal; i++) { //copy values from tempMultVals and tempMultCols to output values and colPos
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

int* CSR::getColumnVector(int col) {
	int* colVector = new int[n];
	int r; //number of values in the current row
	for (int i = 0; i < n; i++)
		colVector[i] = 0;

	bool found;
	int k, j;

	k = 0; //current index of values array
	for (int i = 0; i < n - 1; i++) { //all rows except for the last
		r = rowPtr[i + 1] - rowPtr[i];
		k = rowPtr[i];
		found = false;
		j = 0; //current index of the value in the current row, starting from 0
		while ((j < r) && !found) {
			if (colPos[k] == col) {
				found = true;
				colVector[i] = values[k];
			}
			k++;
			j++;
		}
	}
	int p = rowPtr[n - 1];
	found = false;
	while ((p < (nonZeros)) && (!found)) { //last row
		if (colPos[p] == col) {
			colVector[n - 1] = values[p];
			found = true;
		}
		else
			p++;
	}

	return colVector;
}

int* CSR::getRowVec(int row) {
	int* vector = new int[n];
	for (int i = 0; i < n; i++)
		vector[i] = 0;

	if (row < n - 1) { //any row except the last
		for (int i = rowPtr[row]; i < rowPtr[row + 1]; i++) {
			for (int j = 0; j < m; j++) {
				if (colPos[i] == j)
					vector[j] = values[i];
			}
		}
	}
	else { //last row
		for (int i = rowPtr[row]; i < nonZeros; i++) {
			for (int j = 0; j < m; j++) {
				if (colPos[i] == j)
					vector[j] = values[i];
			}
		}
	}

	return vector;
}

CSR::~CSR() {
	if (values != NULL) delete [] values;
	if (rowPtr != NULL) delete [] rowPtr;
	if (colPos != NULL) delete [] colPos;
	n = 0;
	m = 0;
	nonZeros = 0;
	valIdx = 0;
	colIdx = 0;
	rowIdx = 0;
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

