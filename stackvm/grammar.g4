start : expression EOF;

expression : primary_expr | expr;

primary_expr : constant | OPEN_PAREN expr CLOSE_PAREN;

postfix_expr : primary_expr | identifier OPEN_PAREN argument_list CLOSE_PAREN;

argument_list : expr | argument_list COMMA expr;

unary_expr : postfix_expr | MINUS_OP unary_expr | NOT_OP unary_expr | NOT2_OP unary_expr;
     
multiplicative_expr : unary_expr
                    | multiplicative_expr MULT_OP unary_expr
                    | multiplicative_expr DIV_OP unary_expr
                    | multiplicative_expr MOD_OP unary_expr
                    ;

additive_expr : multiplicative_expr
              | additive_expr PLUS_OP multiplicative_expr
              | additive_expr MINUS_OP multiplicative_expr
              ;

relational_expr : additive_expr
                | relational_expr LT_OP additive_expr
                | relational_expr GT_OP additive_expr
                | relational_expr LTE_OP additive_expr
                | relational_expr GTE_OP additive_expr
                ;

equality_expr : relational_expr
              | equality_expr EQUAL_OP relational_expr
              | equality_expr NOT_EQUAL_OP relational_expr
              | equality_expr NOT_EQUAL2_OP relational_expr
              ;

logical_and_expr : equality_expr | logical_and_expr LOGICAL_AND_OP equality_expr;       

logical_or_expr : logical_and_expr | logical_or_expr LOGICAL_OR_OP logical_and_expr;
                
expr : logical_or_expr;

constant : NUMBER | STRING;

identifier : LETTER | identifier LETTER | identifier DIGIT;

letters : LETTER | letters LETTER;

NOT2_OP : '!';
NOT_OP : 'NOT';
MOD_OP : '%';
MULT_OP : '*';
DIV_OP : '/';
OPEN_PAREN : '(';
CLOSE_PAREN : ')';
COMMA : ',';
MINUS_OP : '-';
PLUS_OP : '+';
LT_OP : '<';
LTE_OP : '<=';
GT_OP : '>';
GTE_OP : '>=';
NOT_EQUAL2_OP : '<>';
EQUAL_OP : '=';
NOT_EQUAL_OP : '!=';
LOGICAL_AND_OP : "AND";
LOGICAL_OR_OP : "OR";
NUMBER : [0-9]+([.][0-9]*)?;
STRING : ('"' ~["\n]* '"');
WHITESPACE : [ \t\r\n]+ -> skip;
LETTER :[A-Z];
DIGIT : [0-9];