import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class NeuralNet {

        public static final double WEIGHT_UPPER =  4;
        public static final double WEIGHT_LOWER = -4;

        public static final int INPUT_LAYER_SIZE  =  258;
        public static final int HIDDEN_LAYER_SIZE = 1000;
        public static final int OUTPUT_LAYER_SIZE =    4;


        public static final Random r = new Random();
        /*
         * ========================= MODEL ==============================
         *
         *
         * INPUT LAYER
         *
         * 200 playing area, 1 and 0 representing occupation +
         * 4 L block orientations +
         * 4 J block orientations +
         * 4 T block orientations +
         * 2 Z block orientations +
         * 2 Y block orientations +
         * 2 I block orientations +
         * 1 O block orientations +
         * 10 x positions +
         * 22 y positions +
         *
         * 7 inpute neurons based on the next block
         * = 258 INPUT NEURONS + 1 bias
         *
         *
         * HIDDEN LAYERS
         *
         * 1000 hidden layers
         *
         *
         *
         * OUTPUT LAYER
         *
         * 1 move left                      +
         * 1 move right                     +
         * 1 rotate in clockwise fashion    +
         * 1 do nothing
         *
         * = 4 OUTPUT NEURONS
         *
         */

        public static void initializeWeights(Individual nn) {
                for (int i = 0; i < nn.chromosome2.length; i++) {
                        for (int j = 0; j < nn.chromosome2[0].length; j++) {
                                nn.chromosome2[i][j] = r.nextGaussian() * (WEIGHT_UPPER - WEIGHT_LOWER)
                                        + WEIGHT_LOWER;
                        }
                }

                for (int i = 0; i < nn.chromosome1.length; i++) {
                        for (int j = 0; j < nn.chromosome1[0].length; j++) {
                                nn.chromosome1[i][j] = r.nextGaussian() * (WEIGHT_UPPER - WEIGHT_LOWER)
                                        + WEIGHT_LOWER;
                        }
                }
                nn.bias1 = r.nextGaussian() * (WEIGHT_UPPER - WEIGHT_LOWER) //input bias
                                        + WEIGHT_LOWER;

                nn.bias2 = r.nextGaussian() * (WEIGHT_UPPER - WEIGHT_LOWER) //hidden layer bias
                                        + WEIGHT_LOWER;

        }

        public static double sigmoid(double x) {
                return 1.0 / (1 + Math.pow(2.17828, -x));
        }

        public static double[] feedForward(Individual nn, double[] input) {
                double[] hiddenNeurons = new double[HIDDEN_LAYER_SIZE];
                double[] outputNeurons = new double[OUTPUT_LAYER_SIZE];

                for (int i = 0; i < nn.chromosome1[0].length; i++) {
                        double crossMultiplySum = 0;
                        for (int j = 0; j < nn.chromosome1.length; j++)
                                crossMultiplySum += nn.chromosome1[j][i] * input[j];

                        hiddenNeurons[i] = sigmoid(crossMultiplySum + nn.bias1);
                }

                for (int i = 0; i < nn.chromosome2[0].length; i++) {
                        double crossMultiplySum = 0;
                        for (int j = 0; j < nn.chromosome2.length; j++)
                                crossMultiplySum += nn.chromosome2[j][i] * hiddenNeurons[j];

                        outputNeurons[i] = sigmoid(crossMultiplySum + nn.bias2);
                }
                return outputNeurons;
        }
}