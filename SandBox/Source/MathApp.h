#pragma once

#include "Core/Core.h"
#include "Core/Application.h"

#include <vector>

class Task1 : public Pengine::Application
{
private:

	double a = 0.0, b = 0.0; // ������� ��������������.
	double h = 0.0; // ���.

	size_t n = 0; // ���������� �����.

	// �������.
	std::function<double(double)> f;

	// ������� �������� x � y.
	std::vector<double> xi, yi;

	// �������.
	double L(double x);

	// ������.
	double P(double x);
public:

	Task1(double a, double b, size_t n, std::function<double(double)> f);

	virtual void OnUpdate() override;
};

class Task2 : public Pengine::Application
{
private:

	double a = 0.0, b = 0.0; // ������� ��������������.
	double h = 0.0; // ���.

	size_t n = 0; // ���������� �����.
	size_t m = 0; // ���������� ��������.

	// �������
	std::function<double(double)> f;

	// �������.
	double Ch();

	// ������-�����.
	double R();

	// X � ������� �������� ��� a � b ��������.
	double X(size_t i, double a, double b);
public:

	Task2(double a, double b, size_t n, size_t m, std::function<double(double)> f);

	virtual void OnUpdate() override;
};