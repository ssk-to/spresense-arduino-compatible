/*
 *  DNNRT.cpp - Spresense Arduino DNN runtime library 
 *  Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @file DNNRT.cpp
 * @author Sony Semiconductor Solutions Corporation
 * @brief Spresense Arduino DNN runtime library 
 * 
 * @details It is a library for use Deep Neural Network (DNN).
 */

#include <Arduino.h>
#include <sdk/config.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <dnnrt/runtime.h>

#include "DNNRT.h"

int
DNNRT::begin(File &nnbfile)
{
  int ret;
  size_t size;

  ret = dnn_initialize(NULL);
  if (ret < 0)
    {
      return ret;
    }

  size = nnbfile.size();
  _network = new nn_network_t;
  ret = nnbfile.read(_network, size);
  if (ret < 0)
    {
      delete _network;
      _network = NULL;
      return -1;
    }

  _rt = new dnn_runtime_t;
  ret = dnn_runtime_initialize(_rt, _network);
  if (ret < 0)
    {
      delete _network;
      delete _rt;

      _network = NULL;
      _rt = NULL;

      return -2;
    }

  // Allocate input data array from network data

  _nr_inputs = dnn_runtime_input_num(_rt);
  _input = new void *[_nr_inputs];

  // Allocate output data variables from network data

  _nr_outputs = dnn_runtime_output_num(_rt);

  return 0;
}

int
DNNRT::end()
{
  dnn_runtime_finalize(_rt);
  dnn_finalize();

  delete _network;
  delete _rt;
  delete[] _input;
  delete[] _output;

  return 0;
}

int
DNNRT::inputVariable(DNNVariable var, unsigned int index)
{
  if (index >= _nr_inputs)
    {
      return -1;
    }

  _input[index] = var.data();

  return 0;
}

DNNVariable&
DNNRT::outputVariable(unsigned int index)
{
  return _output[index];
}

int
DNNRT::forward(void)
{
  return dnn_runtime_forward(_rt, (const void **)_input, _nr_inputs);
}

int
DNNRT::numOfInput(void)
{
  int ret = dnn_runtime_input_num(_rt);
  return ret;
}

int
DNNRT::inputSize(unsigned int index)
{
  int ret = dnn_runtime_input_size(_rt, index);
  return ret;
}

int
DNNRT::inputDimension(unsigned int index)
{
  int ret = dnn_runtime_input_ndim(_rt, index);
  return ret;
}

int
DNNRT::inputShapeSize(unsigned int index, unsigned int dindex)
{
  int ret = dnn_runtime_input_shape(_rt, index, dindex);
  return ret;
}

int
DNNRT::numOfOutput()
{
  int ret = dnn_runtime_output_num(_rt);
  return ret;
}

int
DNNRT::outputSize(unsigned int index)
{
  int ret = dnn_runtime_output_size(_rt, index);
  return ret;
}

int
DNNRT::outputDimension(unsigned int index)
{
  int ret = dnn_runtime_output_ndim(_rt, index);
  return ret;
}

int
DNNRT::outputShapeSize(unsigned int index, unsigned int dindex)
{
  int ret = dnn_runtime_output_shape(_rt, index, dindex);
  return ret;
}

////////////////////////////////////////////////////////////////////////////
// DNNVariable
////////////////////////////////////////////////////////////////////////////

DNNVariable::DNNVariable() : _data(0), _size(0)
{
}

DNNVariable::DNNVariable(unsigned int size) : _data(0), _size(size)
{
  _data = new float[size];
}

DNNVariable::~DNNVariable()
{
  if (_data)
    delete _data;
}
