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
from matplotlib.ticker import FormatStrFormatter

def plot_overhead(csvfile):

    df = pd.read_csv(csvfile)

    df = df[df['testcase'] == 'FullRLNCUnsystematic']
    df = df[df['symbol_size'] == 1500]

    df = df.drop(['symbol_size','testcase'], axis = 1)

    grouped = df.groupby(by=['benchmark','symbols'])

    def compute_overhead(group):

        coded = float(group['coded'].sum())
        used = float(group['used'].sum())

        s = pd.Series([((used/coded) - 1.0)*100], ['overhead'])
        return s

    results = grouped.apply(compute_overhead)
    results = results.reset_index()
    results = results.set_index(['benchmark','symbols'])

    o = results['overhead'].unstack(level=0)

    ax = o.plot(title='overhead', logy=True)
    ax.yaxis.set_major_formatter(FormatStrFormatter('%.01f'))

    plt.show()

if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--csv_file', dest='csvfile', action='store',
        help='the .csv file written by gauge benchmark',
        default='out.csv')

    args = parser.parse_args()

    plot_overhead(args.csvfile)
