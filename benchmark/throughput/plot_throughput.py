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

def plot_throughput(csvfile):

    df = pd.read_csv(csvfile)

    def density_to_string(density):
        if not np.isnan(density):
            return " density {}".format(density)
        else:
            return ""

    # df['density'] = df['density'].map(density_to_string)
    # df['test'] = df['testcase'].map(str) + '.' + df['benchmark'] + df['density']

    plot_groups = list(df.groupby(by=['testcase', 'symbol_size', 'type']))

    # Group the plots
    for (test, symbol_size, type), df in plot_groups:

        def density_to_string(density):
            if not np.isnan(density):
                return "density {}".format(density)
            else:
                return ""

        df['benchmark'] = df['benchmark'] + ' ' + df['density'].map(density_to_string)

        group = df.groupby(by = ['benchmark', 'symbols'])

        def compute_throughput(group):
            s = group['throughput']
            s = pd.Series([s.mean(), s.std()], ['mean','std'])
            return s

        df = group.apply(compute_throughput)
        df = df.unstack(level=0)

        df['mean'].plot(title="Throughput {} {} p={}B".format(test, type, symbol_size),
                        kind='bar')


    plt.show()

    print encoder
    print decoder

if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--csv_file', dest='csvfile', action='store',
        help='the .csv file written by gauge benchmark',
        default='out.csv')

    args = parser.parse_args()

    plot_throughput(args.csvfile)
