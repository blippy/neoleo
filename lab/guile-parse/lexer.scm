(define (lex inputs)
  (let loop ((acc '()) (inputs (string->list inputs)))
    (if (null? inputs)
	(reverse acc)
	(let* ((c (car inputs))
	       (rem (cdr inputs)))
	  (cond
	    ((char-whitespace? c) (loop acc inputs))
	    (else #t))))))

(lex "23.4foo(bar)")
