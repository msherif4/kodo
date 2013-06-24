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
import re
from matplotlib.ticker import FormatStrFormatter

def plot(csvfile, saveas):

    df = pd.read_csv(csvfile)

    def density_to_string(density):
        if not np.isnan(density):
            return " density {}".format(density)
        else:
            return ""

    # Combine the testcase and benchmark columns into one (used for labels)
    if not 'density' in df:
        df['test'] = df['testcase'].map(str) + '.' + df['benchmark']

        df = df.drop(['testcase','benchmark'], axis = 1)
    else:
        df['test'] = df['testcase'].map(str) + '.' + df['benchmark'] +\
                     ' ' + df['density'].map(density_to_string)
        df = df.drop(['testcase','benchmark', 'density'], axis = 1)

    grouped = df.groupby(by=['test','symbols', 'symbol_size', 'erasure'])

    def compute_rank_cdf(group):
        symbols = group['symbols'].unique()

        # We group by symbols so there should only be one value
        assert(len(symbols) == 1)
        symbols = symbols[0]

        labels = ['rank {}'.format(i) for i in range(symbols)]
        values = []

        for l in labels:
            values.append(group[l].mean() - 1.0)

        return pd.Series(values, range(symbols), name = 'linear dept')


    df = grouped.apply(compute_rank_cdf).head()
    df = df.transpose()

    for t in df:
        p = df[t]
        plt.figure()
        p.plot(style='-x', title="{}".format(t))

        plt.xlabel('Rank of decoder')
        plt.ylabel('Linear dependent packets')

        if saveas:

            setup = "{}".format(t)
            setup = re.sub('[^0-9a-zA-Z ]+', '', setup).lower().replace(' ','_')

            filename = "linear_dependency_vs_rank_{}.{}".format(
                setup, saveas)

            print("Saving", filename)
            plt.savefig(filename)


    if not saveas:
        plt.show()

    return


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

    plot(args.csvfile, args.saveas)
