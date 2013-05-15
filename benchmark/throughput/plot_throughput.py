# This simple script plots the csv file produced by the throughput
# benchmark, if you make some cool visualizations please send a patch

import argparse
import pandas as pd
import pylab as plt

def group_testcase(dataframe):
    groups = list(dataframe.groupby(by='testcase'))

    for name, g in groups:
        group_symbol_size(name, g)

def group_symbol_size(testcase, dataframe):
    groups = list(dataframe.groupby(by='symbol_size'))

    for name, g in groups:
        plot_group(testcase, name, g)

def plot_throughput(csvfile):

    df = pd.read_csv(csvfile)
    group_testcase(df)

def plot_group(testcase, symbol_size, dataframe):

    # We don't need the symbol_size,testcase columns anymore
    # since we already grouped on these
    dataframe = dataframe.drop(['symbol_size','testcase'], axis = 1)

    grouped = dataframe.groupby(by=['benchmark','symbols', 'type'])

    def compute_throughput(group):
        s = group['throughput']
        s = pd.Series([s.mean(), s.std()], ['mean','std'])
        return s

    results = grouped.apply(compute_throughput)
    results = results.reset_index()

    encoder = results[(results['type'] == 'encoder')]
    decoder = results[(results['type'] == 'decoder')]

    # We don't need the type,symbol_size,testcase columns anymore
    encoder = encoder.drop(['type'], axis = 1)
    decoder = decoder.drop(['type'], axis = 1)

    encoder = encoder.set_index(['benchmark','symbols'])
    decoder = decoder.set_index(['benchmark','symbols'])

    f, ((ax1,ax2),(ax3,ax4)) = plt.subplots(2, 2)
    encoder['mean'].unstack(level=0).plot(kind='bar', ax = ax1, title='Encoder Mean')
    encoder['std'].unstack(level=0).plot(kind='bar', ax = ax2)
    decoder['mean'].unstack(level=0).plot(kind='bar', ax = ax3)
    decoder['std'].unstack(level=0).plot(kind='bar', ax = ax4)

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
