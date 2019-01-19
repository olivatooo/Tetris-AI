import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class Individual implements Comparable<Individual>{
        /*
         * an Individual is simply a neural network, along with some biological functions
         * weights, from -> to
         * there are two chromosomes, one for first, one for second layer
         */
        double[][] chromosome1 = null;
        double[][] chromosome2 = null;
        double bias1, bias2;
        int fitness = 0;

        public Individual() {
                chromosome1 = new double[NeuralNet.INPUT_LAYER_SIZE][NeuralNet.HIDDEN_LAYER_SIZE];
                chromosome2 = new double[NeuralNet.HIDDEN_LAYER_SIZE][NeuralNet.OUTPUT_LAYER_SIZE];
                NeuralNet.initializeWeights(this);
        }

        public Individual(double[][] chromosome1, double[][] chromosome2) {
                this.chromosome1 = chromosome1;
                this.chromosome2 = chromosome2;
        }

        public int compareTo(Individual a) {
                if (this.fitness < a.fitness)
                        return 1;
                else if (this.fitness == a.fitness)
                        return 0;
                else
                        return -1;
        }
}
