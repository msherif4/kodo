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

    f, ((ax1,ax2),(ax3,ax4)) = plt.subplots(2, 2, sharex=True)
    l1 = encoder['mean'].unstack(level=0).plot(kind='bar', ax = ax1, title='Encoder Mean', legend=False)
    l2 = encoder['std'].unstack(level=0).plot(kind='bar', ax = ax3, title='Encoder Std dev.', legend=False)
    l3 = decoder['mean'].unstack(level=0).plot(kind='bar', ax = ax2, title='Decoder Mean', legend=False, sharey=ax1)
    l4 = decoder['std'].unstack(level=0).plot(kind='bar', ax = ax4, title='Decoder Std dev.', legend=False,sharey=ax3)

    handles, labels = ax1.get_legend_handles_labels()
    #ax1.legend(bbox_to_anchor=(1.1, 1.05))
    t = f.legend(handles, labels,ncol=4, mode='expand')
    #t.set_y(1.09)

    f.tight_layout()
    plt.subplots_adjust(top=0.86)
    #f.legend((l3, l4), ('Line 3', 'Line 4'), 'upper right')
    #f.legend(loc="best")
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
