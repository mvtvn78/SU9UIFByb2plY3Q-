package com.maivantien.lab304_new.service;
import java.util.List;

import com.maivantien.lab304_new.dto.ProductDto;
import com.maivantien.lab304_new.entity.Product;
public interface ProductService {
Product createProduct(ProductDto product);
Product getProductById(Long productId);
List<Product> getAllProducts();
Product updateProduct(Product product);
void deleteProduct(Long productId);
}
