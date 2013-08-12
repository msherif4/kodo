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
from matplotlib.backends.backend_pdf import PdfPages as pp

def plot_throughput(args):

    df = pd.read_csv(args.csvfile)

    plot_groups = list(df.groupby(by=['testcase', 'symbol_size', 'type']))

    if args.outfile:
        pdf = pp(args.outfile)

    # Group the plots
    for (test, symbol_size, type), df in plot_groups:

        def density_to_string(density):
            if not np.isnan(density):
                return "density {}".format(density)
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


        group = df.groupby(by = ['test', 'symbols'])

        def compute_throughput(group):
            s = group['throughput']
            s = pd.Series([s.mean(), s.std()], ['mean','std'])
            return s

        df = group.apply(compute_throughput)
        df = df.unstack(level=0)

        df['mean'].plot(title="Throughput {} {} p={}B".format(
            test, type, symbol_size), kind='bar')

        if args.outfile:
            pdf.savefig(transparent=True)

    if args.outfile:
        pdf.close()
    else:
        plt.show()

if __name__ == '__main__':

    parser = argparse.ArgumentParser()

    parser.add_argument(
        '--csv_file', dest='csvfile', action='store',
        help='the .csv file written by gauge benchmark',
        default='out.csv')
    parser.add_argument(
        '--out_file', dest='outfile', action='store',
        help='the pdf to save the plots to',
        default=None)

    args = parser.parse_args()

    plot_throughput(args)
