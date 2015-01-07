/**
 *  @file Condensation.h
 *
 * @section DESCRIPTION
 * Condensation algorithm on LarvaModels.
 * Based on the original implementation of the condensation algorithm.
 */

#pragma once

#include "..//QTTestVs10//LarvaModel.h"

#include "LarvaModelController.h"
#include "LarvaModelManager.h"
#include "Painter.h"
/**
 * Condensation algorithm on LarvaModels.
 * Based on the original implementation of the condensation algorithm.
 */
class Condensation
{
public:
	Condensation(void);
	~Condensation(void);

	/**
    * Select a sample based on which a new sample may be generated.
	*
	* @param _largest_cumulative_prob	Largest fitness
	* @param _currentDescriptors		Available samples
	* @param _cumul_prob_array			Fitness of the samples
	* @param _sampleCount				Number of available samples
	* @return Picked Sample
	*/
	LarvaModel	pick_base_sample(
		double							_largest_cumulative_prob,
		vector<LarvaModel>			&	_currentDescriptors, 
		vector<double>				&	_cumul_prob_array,
		int								_sampleCount);	

	/**
    * Predicts a LarvaModel based on given previous model.
	* Uses random gaussian to generate possible progression
	* and displacement parameters.
    * 
    * @param _oldObject	Previous model used as base
	* @return Possible	LarvaModel
	*/
	LarvaModel	predict_sample_position(
		LarvaModel					*	_oldObject);

	/**
    * Predicts a LarvaModel based on given previous model.
	* Uses random gaussian to generate possible progression
	* and displacement parameters.
    * 
    * @param _oldObject				Previous model used as base
    * @param _obsDensityData		Matrix containing observation density information
    * @param mod_cumulTotal			Cumulative score
    * @param mod_sampleContainer	Container for all samples
    * @param mod_weightContainer	Container for all scores
    * @param mod_cumulProbContainer	Container for cumulative scores
	*/
	void predict_sample_position(
		LarvaModel					*	_oldObject, 
		const Mat					&	_obsDensityData,
		double						&	mod_cumulTotal,
		vector<LarvaModel>			&	mod_sampleContainer, 
		vector<double>				&	mod_weightContainer,
		vector<double>				&	mod_cumulProbContainer);

	/**
    * Evaluates a given LarvaModel based on the provided observation data.	
    *
    * @param	_larva					Model to be checked.
	* @param	_image					Single channel observation: dark intensities correspond to background.
	* @param	_underFillWeightFactor	Factor for punishing models that use less space than possible.
	* @return	Value between 0 and 1 describing the given models likelihood.
	*/
	double evaluate_observation_density(
		LarvaModel					&	_larva, 
		const Mat					&	_image,
		const float					_underFillWeightFactor = 0.75f);

	/**
    * Creates an energy-matrix for given larva
    *
    * @param	_prevLarva	
	* @param	_currentImg	
	* @param	_sampleCount
	* @return	Energymatrix
	*/
	Mat	getEnergy(
		LarvaModel					*	_prevLarva,
		const Mat					&	_currentImg,
		const int						_sampleCount);

	/**
    * Computes a weighted mean from a set of models and a list of corresponding weights.
	* The new model is set to be a child of model [prev].
    *
    * @param	_bases		Set of LarvaModels
	* @param	_weights	Weights of models from [bases]
	* @return	Weighted-Mean-LarvaModel
	*/
	LarvaModel* computeMeanModel(
		const vector<LarvaModel>	&	_bases, 
		const vector<double>		&	_weights);		

	LarvaModel* chooseBestModel(		
		const vector<LarvaModel>	&	_bases, 
		const vector<double>		&	_weights,
		double						&	out_bestFitness);		

private:
	/**
	* Random number generator.
	*/
	RNG									mRandom;

	/**
	* Object for transforming LarvaModels.
	*/
	GeneralTrackerFuncs::LarvaModelController	mLarvaController;
};

