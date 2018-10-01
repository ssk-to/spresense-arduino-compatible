/*
 *  DNNRT.h - Spresense Arduino DNN runtime library 
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

#ifndef Dnnrt_h
#define Dnnrt_h

#include <Arduino.h>

#include <dnnrt/runtime.h>

class DNNVariable; // forward reference

/**
 * @file DNNRT.h
 * @author Sony Semiconductor Solutions Corporation
 * @brief Spresense Arduino DNN runtime library 
 * 
 * @details 
 *
 * 
 */

class DNNRT {

public:
  DNNRT() {};
  ~DNNRT() {};

  /**
   * Initialize runtime object from .nnb file
   *  
   * User must be generate a network data file (.nnb) by NNabla before run this library.
   *  
   * About NNabla:
   *  https://nnabla.readthedocs.io/en/latest/python/index.html
   * About NNB file:
   *  https://nnabla.readthedocs.io/en/latest/python/file_format_converter/file_format_converter.html
   */  
  int begin(File &nnbfile);

  /**
   * Finalize runtime object
   */
  int end();

  /**
   * Set input data at index
   */
  int inputVariable(DNNVariable var, unsigned int index);

  /**
   * Get output data at index
   */
  DNNVariable& outputVariable(unsigned int index);

  /**
   * Execute forward propagation
   */
  int forward(void);

  /**
   * Get number of network inputs
   */
  int numOfInput();

  /**
   * Size of input data at index
   */
  int inputSize(unsigned int index);

  /**
   * Get dimension of input data at index
   */
  int inputDimension(unsigned int index);

  /**
   * Shape size at shape index
   */
  int inputShapeSize(unsigned int index, unsigned int shapeindex);

  /**
   * Get number of network outputs
   */
  int numOfOutput();

  /**
   * Size of output data at index
   */
  int outputSize(unsigned int index);

  /**
   * Get dimension of output data at index
   */
  int outputDimension(unsigned int index);

  /**
   * Shape size at output shape index
   */
  int outputShapeSize(unsigned int index, unsigned int shapeindex);

private:
  dnn_runtime_t *_rt;            // DNN runtime context
  nn_network_t  *_network;       // Network data from .nnb file

  void         **_input;         // Input data array
  int            _nr_inputs;     // Number of input data
  DNNVariable   *_output;        // Output data array
  int            _nr_outputs;    // Number of output data
};

class DNNVariable {

public:
  friend class DNNRT;

  DNNVariable();
  DNNVariable(unsigned int size);
  ~DNNVariable();

  unsigned int size() {
    return _size;
  }

  const float& operator[](unsigned int index) const {
    return _data[index];
  }

  float *data() {
    return _data;
  }

private:
  float *_data;
  unsigned int _size;
};

#endif
