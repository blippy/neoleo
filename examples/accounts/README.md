
You can even process your accounts with groff. For example
```
neoleo -H <<<tbl accts-out.oleo | groff -T ascii -t > accts.txt
```

produces the output below

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





























