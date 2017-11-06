Ever wanted to keep a simple set of accounts? Well, neoleo
makes that possible.

## Transaction data

Let's keep our transaction data in a spreadsheet, let's call it
`trans.oleo`:

```
01 Jan 2017     cash    open    100     Opening balance
01 Jan 2017     bank    open    2000.12 Opening balance
01 Jan 2017     visa    open    0       Opening balance
02 Sep 2017     shop    cash    20.66   Walmart
03 Sep 2017     car     visa    123.40  Petrol
03 Sep 2017     bank    wages   200     payday
04 Sep 2017     visa    bank    5       pay off credit card
04 Sep 2017     shop    cash    20.66   Walmart
```

Here, I have chosen to have the spreadsheet laid over 5 columns. Column 1 is the date, 
column 2 is the "debit account", column 3 is the "credit account", column 4 is the
amount, and column 5 is a description.


## Accounts template

Then we need to set up a template for how we would like the accounts to appear.
I call this `accts-plate.oleo`. You can lay out your own report however you
wish. I have used `set-var` to set cells with names. So there is a cell
named "wages", another one for "gifts", and so on.

## Process

Now we need to get the data from `trans.oleo` into `accts-plate.oleo`. What we do 
is output the transaction data as tab-separated values. We use `awk` to accumulate
the account balances, and print a set of commands for the template to use.

I have called this file `process`, which looks like this:

```
#!/usr/bin/env bash

neoleo -H trans.oleo <<<type-dsv  > /tmp/accts1


awk -F '\t' </tmp/accts1 '
{tot[$2] += $4 ; tot[$3] -= $4; }

END { 
	for(key in tot) { 
		print "goto-cell " key;
		print "edit-cell " tot[key];
	}
	print "w"
	print "q"

}
' >/tmp/accts2

cp accts-plate.oleo accts-out.oleo

neoleo -H    accts-out.oleo </tmp/accts2
```

It outputs a file called `accts-out.oleo`.

## Viewing

You can view the output directly by typing
```
neoleo -H accts-out.oleo
```

But wait! You can go one step further by turning it simple plain text.
Neoleo can output text in groff's tbl format. So you can do things
like produces PDFs of your accounts.

For example, let's show the accounts in plain-text format:
```
neoleo -H <<<tbl accts-out.oleo | groff -T ascii -t > accts.txt
```

It produces the output:


```
Income and Expenses

             INCOME
              Wages       -200
              Gifts          0
                      --------       -200



           EXPENSES
              Motor     123.40
               Shop      41.32
                      --------     164.72
                                 --------
            SURPLUS                -35.28
                                 ========


      BALANCE SHEET

             ASSETS
               Bank    2195.12
               Cash      58.68
                      --------
                       2253.80
                      ========

        LIABILITIES
               Visa    -118.40
               Open   -2100.12
            Surplus     -35.28
                      --------
                      -2253.80
                      ========

```





























