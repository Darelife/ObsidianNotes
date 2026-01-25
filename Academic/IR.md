Zips and heaps law
How can we predict the size of ...
Basic design
Inverted index
indexing
tf idf


## Page Rank

How imp is each node in the network??
V.I.N (very imp nodes)

(Crawl the webpages.....(should follow robot.txt (or should it?)))
1. Start with a set of pages.
2. Check all the backlinks, and determine the link structure.
3. Assign each page an initial rank of 1/N.
4. Successively update the rank of each page by ading up the weight of every page that links to it divided by the number of links emanating from the referrinig page.
   
   C has 3 outgoing links
   E has 2 incoming links from C and D
   D has 2 outgoing links (and 1 incoming from C)
   PR(E) = PR(C)/3 + PR(D)/2 = sigma (incoming page rank / number of outgoing links from it)
   
   if PR(C) = PR(D) = PR(E),
   PR(E) = 0.2/3 + 0.2/2 = 0.17....and we'll keep doing this for all nodes
   
   Now if we have a page with no outgoing links, (sink node). We'll randomly jump in that case.
   
   We repeat this algo until the page rank stabilizes.

