# This simple script plots the csv file produced by the throughput
# benchmark, if you make some cool visualizations please send a patch.
#
# To ensure the plots look right you need to use pandas version >= 0.11.0.
# You can check which version you are on by running:
# >>> import pandas
# >>> pandas.__version__

import argparse
import pandas as pd
import pylab as plt
import numpy as np
from matplotlib.ticker import FormatStrFormatter

def plot_symbols_needed(csvfile, saveas):

    df = pd.read_csv(csvfile)

    # Combine the testcase and benchmark columns into one (used for labels)
    #df['test'] = df['testcase'].map(str) + '.' + df['benchmark']
    #df = df.drop(['testcase','benchmark', 'unit', 'iterations'], axis = 1)
    df = df.drop(['unit', 'iterations'], axis = 1)

    df['extra'] = df['used'] - df['symbols']

    # Group the plots
    plot_groups = list(df.groupby(by=['testcase', 'symbol_size','symbols','erasure']))

    for (testcase, symbol_size, symbols, erasure), df in plot_groups:

        df = df.pivot(index='runs', columns='benchmark', values='extra')

        df.plot(title="Extra symbols needed for decoding {}".format(testcase),
                style = ['+','o','^','s','p','x','h','D', '*'], alpha=0.75)

        if saveas:
            filename = "decoding_scatter_{}_{}_{}_{}.{}".format(
                testcase.lower(), symbols, symbol_size,
                str(erasure).replace('.',''), saveas)

            print("Saving", filename)
            plt.savefig(filename)

        plt.xlabel('Test run')
        plt.ylabel('Extra symbols needed for decoding')

    if not saveas:
        plt.show()

if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--csv_file', dest='csvfile', action='store',
        help='the .csv file written by gauge benchmark',
        default='out.csv')

    parser.add_argument(
        '--saveas', dest='saveas', action='store',
        help='Figures will be saved as the specified type e.g. --saveas=pdf',
        default=None)


    args = parser.parse_args()

    plot_symbols_needed(args.csvfile, args.saveas)
