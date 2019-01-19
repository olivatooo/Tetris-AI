import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class Genetic {

      public static final int GENERATION    =  1000;
      public static final int SAMPLE_SIZE   =    50; //sample size of most fit individuals
      public static final int POPULATION    =   100;
      public static final int MUTATION_RATE =    10;
      public static final Random r = new Random();

      public static Individual[] population = null;

      /*
       * ========================= NOTES ==============================
       *
       * - Population starts out with POPULATION and continues on like this for n = 1 to n = GENERATION
       *
       * - Each generation will be the zygote formed from binary gametes (2 parents)
       *
       * - Each parent will product 2 offspring with 2 other partners
       *
       * - Because of this, each generation will always have the same POPULATION, thereby decreasing the
       *   complexity of this problem
       *
       * - Think of each individual as a neural network, don't get confused by the nn class, thats for
       *   utilities
       *
       * - For mutation we can use this formula, w = w + (w * r) where r is a random number sampled
       *   from a normal distribution for biological accuracy
       */

      public static void initializePopulation() {
            //initialize the starting population
            population = new Individual[POPULATION];
            for (int i = 0; i < POPULATION; i++)
                population[i] = new Individual(); //set to random
      }

      public static void reset() {
            Tetris.level = 0;
            Tetris.score = 0;
      }

      public static double[] getInput() { //get the input for the neural network
            double[] inputLayer = new double[NeuralNet.INPUT_LAYER_SIZE];
            //get block orientation
            for (int i = 1; i <= 19; i++) {
                if (Tetris.type == i)
                      inputLayer[i - 1] = 1;
            }

            for (int i = 1; i <= 7; i++) {
                if (Tetris.nextType == i)
                      inputLayer[19 + i - 1] = 1;
            }

            //get position
            for (int i = 0; i < Tetris.HEIGHT; i++) {
                if (Tetris.y == i)
                      inputLayer[26 + i] = 1;
            }

            for (int i = 0; i < Tetris.WIDTH; i++) {
                if (Tetris.x == i)
                      inputLayer[26 + Tetris.HEIGHT + i] = 1;
            }


            //get tetris game map
            int ptr = 26 + Tetris.HEIGHT + Tetris.WIDTH;
            for (int i = 2; i < Tetris.board.length; i++) {
                for (int j = 0; j < Tetris.board[0].length; j++) {
                      if (Tetris.board[i][j] != 0)
                            inputLayer[ptr] = 1;
                      ptr++;
                }
            }
            return inputLayer;
      }

      public static Individual crossOver(Individual a, Individual b) {
            Individual child = new Individual();

            for (int i = 0; i < a.chromosome1.length/2; i++) {
                for (int j = 0; j < a.chromosome1[0].length; j++)
                      child.chromosome1[i][j] = a.chromosome1[i][j];
            }
            for (int i = b.chromosome1.length/2; i < b.chromosome1.length; i++) {
                for (int j = 0; j < b.chromosome1[0].length; j++)
                      child.chromosome1[i][j] = b.chromosome1[i][j];
            }

            for (int i = 0; i < a.chromosome2.length/2; i++) {
                for (int j = 0; j < a.chromosome2[0].length; j++)
                      child.chromosome2[i][j] = a.chromosome2[i][j];
            }
            for (int i = b.chromosome2.length/2; i < b.chromosome2.length; i++) {
                for (int j = 0; j < b.chromosome2[0].length; j++)
                      child.chromosome2[i][j] = b.chromosome2[i][j];
            }

            child.bias1 = a.bias1;
            child.bias2 = b.bias2;

            return child;
      }

      public static void mutate(Individual nn) { //scamble genes to create more genetic diversity
            for (int i = 0; i < nn.chromosome1.length; i ++) {
                for (int j = 0; j < nn.chromosome1[0].length; j++) {
                      if ((int)(Math.random() * MUTATION_RATE) == 0) {
                            nn.chromosome1[i][j] = nn.chromosome1[i][j] +
                                (nn.chromosome1[i][j] * r.nextGaussian());
                      }
                }
            }

            for (int i = 0; i < nn.chromosome2.length; i ++) {
                for (int j = 0; j < nn.chromosome2[0].length; j++) {
                      if ((int)(Math.random() * MUTATION_RATE) == 0) {
                            nn.chromosome2[i][j] = nn.chromosome2[i][j] +
                                (nn.chromosome2[i][j] * r.nextGaussian());
                      }
                }
            }
      }

      public static void breed() {

            System.out.print("AVERAGE: ");
            int sum = 0;
            for (int i = 0; i < population.length; i++)
                sum += population[i].fitness;
            System.out.println((sum / (double)population.length));

            Arrays.sort(population);
            System.out.println("SELECTED: ");
            for (int i = 0; i < SAMPLE_SIZE; i++)
                System.out.print(population[i].fitness + " ");
            System.out.println();

            int childPtr = 0; //points to where the next spot for a child is
            Individual[] nextGen = new Individual[POPULATION];
            for (int i = 0; i < SAMPLE_SIZE; i++) {
                int partner = (int)(Math.random() * SAMPLE_SIZE);

                nextGen[childPtr] = crossOver(population[i], population[partner]);
                childPtr++;

                //lets have each fit organism have two children while mating once
                nextGen[childPtr] = crossOver(population[partner], population[i]);
                childPtr++;
            }

            for (int i = 0; i < nextGen.length; i++)
                mutate(nextGen[i]);

            population = nextGen; //the old generation dies off here
      }

      public static double squarePreserveSign(int balanceCounter) {
            if (balanceCounter == 0)
                return 0;
            return Math.pow(balanceCounter, 2) * (Math.abs(balanceCounter) / balanceCounter);
      }

      public static int calculateFitness() { //we have this method because simply using score as fitness
                                 // is not enough, we need to calculate wells
            int fitness = Tetris.score; //first get score
            for (int i = 2; i < Tetris.HEIGHT; i++) {
                int balanceCounter = 0;
                for (int j = 0; j < Tetris.WIDTH; j++) {
                      if (Tetris.board[i][j] == 0)
                            balanceCounter--;
                      else
                            balanceCounter++;
                }
                fitness += squarePreserveSign(balanceCounter);
            }

            for (int j = 0; j < Tetris.WIDTH; j++) {
                int balanceCounter = 0;
                for (int i = 2; i < Tetris.HEIGHT; i++) {
                      if (Tetris.board[i][j] == 0)
                            balanceCounter--;
                      else
                            balanceCounter++;
                }
                fitness += squarePreserveSign(balanceCounter);
            }
            return fitness;
      }

      public static void main(String[] args) {

            initializePopulation();
            for (int j = 0; j < GENERATION; j++) {
                Hud.dispGen = j;
                for (int i = 0; i < POPULATION; i++) {

                      Tetris.importHighScores();
                      Gui.init();
                      Gui.refresh();
                      Tetris.nextType = (int)(Math.random() * 7 + 1);
                      Tetris.generate();
                      while (!Tetris.endGameChecker()) {
                            //printBoard();
                            //System.out.println();
                            //game mechanics printing
                            boolean spawn = Tetris.updateBoard();
                            Tetris.delay(Tetris.delay); //give player a chance to move
                            if (spawn && Tetris.checkBoard()) { //checkboard because you can move out of a hit == placement
                                Tetris.freeze(); //first freeze active
                                Tetris.updateTetris(); //update to see if there is a tetris after placement
                                Tetris.generate();
                                //also add score here for ever block you put down
                                Tetris.score += 1;
                            }



                            // ============ TEST AI
                            Individual testPlayer = population[i];
                            double[] output = NeuralNet.feedForward(testPlayer, getInput());
                            double maxAmt = 0;
                            int ptr = 0;
                            for (int k = 0; k < output.length; k++) {
                                if (maxAmt < output[k]) {
                                      maxAmt = output[k];
                                      ptr = k;
                                }
                            }

                            //TESTING -------------------
                            //System.out.println(Arrays.toString(output));
                            //System.out.println(Tetris.otype);
                            //System.out.println(ptr);

                            switch (ptr) {
                            case 0:
                                Tetris.rotate();
                                break;
                            case 1:
                                Tetris.translateLeft();
                                break;
                            case 2:
                                Tetris.translateRight();
                                break;
                            case 3:
                                break;
                            }


                            Gui.refresh();
                      }

                      population[i].fitness = calculateFitness(); //get the fitness for this neural net
                      Tetris.exportHighScores();
                      reset();
                }

                //per generation code here
                breed();
            }
      }
}