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

def plot_overhead(csvfile):

    df = pd.read_csv(csvfile)

    # Combine the testcase and benchmark columns into one (used for labels)
    df['test'] = df['testcase'].map(str) + '.' + df['benchmark']
    df = df.drop(['testcase','benchmark'], axis = 1)

    grouped = df.groupby(by=['test','symbols', 'symbol_size', 'erasure'], axis=0)

    def compute_cdf(group):

        # Get the extra symbols needed for decoding
        coded = group['used'] - group['symbols']

        # Change to float
        coded = coded.astype(float)

        # Count how many runs needed how many extra symbols
        counts = coded.value_counts()
        counts = counts.order(ascending=False)

        # Re-index so we have a linear index with counts for
        # every index value
        values = range(15)
        counts = counts.reindex(index = values, fill_value = 0)

        # Compute the decoding probability
        prob = counts / float(counts.sum())

        # cdf
        cdf = list(prob.cumsum())

        s = pd.Series(cdf, range(len(cdf)))
        return s

    # Compute the cdf for each of the groups
    results = grouped.apply(compute_cdf)
    results = results.reset_index()

    # Group the plots
    plot_groups = list(results.groupby(by=['symbol_size','symbols','erasure']))

    for (symbol_size, symbols, erasure), dataframe in plot_groups:

        # Just discard the not needed columns
        dataframe = dataframe.drop(['symbol_size','symbols','erasure'], axis = 1)

        # Fix the index to the tests
        dataframe = dataframe.set_index(['test'])

        # Transpose the rows and columns so that the extra packet columns
        # becomes rows
        dataframe = dataframe.transpose()
        dataframe.reset_index()
        dataframe.plot(title="symbols={}, symbol size={}, erasures={}".format(
            symbol_size, symbols, erasure))

    plt.show()

if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--csv_file', dest='csvfile', action='store',
        help='the .csv file written by gauge benchmark',
        default='out.csv')

    args = parser.parse_args()

    plot_overhead(args.csvfile)
