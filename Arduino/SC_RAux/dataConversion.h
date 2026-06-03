  unsigned long longVal = 0;
  switch (vars[p]->type) {  
    case f16var::INT:  
        longVal = vars[p]->value.valI;  
        break;  
    case f16var::CHAR:  
        longVal = vars[p]->value.valC;  
        break;  
    case f16var::STRING:  
        longVal = atol((*vars[p]->value.valString).c_str());  
        break;  
    case f16var::LONG:  
        longVal = vars[p]->value.valL;  
        break;  
  } 
 